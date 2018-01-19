
#include "stdafx.h"
#include "myGMM.H"

static const int defaultNMixtures = 5;//Ĭ�ϻ��ģ�͸���
//static const int defaultHistory = 200;//Ĭ����ʷ֡��
static const double defaultBackgroundRatio = 0.7;//Ĭ�ϱ�������
static const double defaultVarThreshold = 2.5*2.5;//Ĭ�Ϸ�������
static const double defaultNoiseSigma = 30*0.5;//Ĭ����������
static const double defaultInitialWeight = 0.05;//Ĭ�ϳ�ʼȨֵ
//���������Ĺ��캯����ʹ��Ĭ��ֵ  
float tmpabs(float a)
{
	if (a>=0)return a;
	else return -a;
}
uchar THDKEY=10;

MyGMM::MyGMM()
{
	frameSize = Size(0,0);
	frameType = 0;

	nframes = 0;
	nmixtures = defaultNMixtures;
	//history = defaultHistory;
	varThreshold = defaultVarThreshold;
	backgroundRatio = defaultBackgroundRatio;
	noiseSigma = defaultNoiseSigma;
}
//�������Ĺ��캯����ʹ�ò�����������ֵ    
MyGMM::MyGMM(int _history, int _nmixtures,
	double _backgroundRatio,
	double _noiseSigma)
{
	frameSize = Size(0,0);
	frameType = 0;

	nframes = 0;
	nmixtures = min(_nmixtures > 0 ? _nmixtures : defaultNMixtures, 8);//���ܳ���8�����������Ĭ�ϵ�
//	history = _history > 0 ? _history : defaultHistory;//����С��0���������Ĭ�ϵ�
	varThreshold = defaultVarThreshold;//����ʹ��Ĭ�ϵ�
	backgroundRatio = min(_backgroundRatio > 0 ? _backgroundRatio : 0.95, 1.);//�������ޱ������0��С��1������ʹ��0.95
	noiseSigma = _noiseSigma <= 0 ? defaultNoiseSigma : _noiseSigma;//�������޴���0
}

MyGMM::~MyGMM()
{
}


void MyGMM::initialize(Size _frameSize, int _frameType)
{
	frameSize = _frameSize;
	frameType = _frameType;
	nframes = 0;

	int nchannels = CV_MAT_CN(frameType);
	CV_Assert( CV_MAT_DEPTH(frameType) == CV_8U );

	// for each gaussian mixture of each pixel bg model we store ...
	// the mixture sort key (w/sum_of_variances), the mixture weight (w),
	// the mean (nchannels values) and
	// the diagonal covariance matrix (another nchannels values)
	bgmodel.create( 1, frameSize.height*frameSize.width*nmixtures*(2 + 2*nchannels), CV_32F );//��ʼ��һ��1��*XX�еľ���
	//XX����������ģ�ͼ�����*��*���ģ�͵ĸ���*��1�����ȼ��� + 1��Ȩֵ�� + 2����ֵ + ��� * ͨ������
	bgmodel = Scalar::all(0);//����
}

//��Ϊģ�棬���ǿ��ǵ��˲�ɫͼ����Ҷ�ͼ���������    
template<typename VT> struct MixData
{
	float sortKey;
	float weight;
	VT mean;
	VT var;
};


static void process8uC1( const Mat& image, Mat& fgmask, double learningRate,
	Mat& bgmodel, int nmixtures, double backgroundRatio,
	double varThreshold, double noiseSigma , Mat GMM_mask,bool nomask)
{
	int x, y, k, k1, rows = image.rows, cols = image.cols;
	float alpha = (float)learningRate, T = (float)backgroundRatio, vT = (float)varThreshold;//ѧϰ���ʡ��������ޡ���������
	int K = nmixtures;//���ģ�͸���
	MixData<float>* mptr = (MixData<float>*)bgmodel.data;

	const float w0 = (float)defaultInitialWeight;//��ʼȨֵ
	const float sk0 = (float)(w0/(defaultNoiseSigma*2));//��ʼ���ȼ�
		const float var0 = (float)(defaultNoiseSigma*defaultNoiseSigma*4);//��ʼ����
	//const float var0 = 25.0;
		const float minVar = (float)(noiseSigma*noiseSigma);//��С����
	//const float minVar = 15.0;

	for( y = 0; y < rows; y++ )
	{
		const uchar* src = image.ptr<uchar>(y);
		uchar* dst = fgmask.ptr<uchar>(y);
		uchar* GmmMask = GMM_mask.ptr<uchar>(y);
		if( alpha > 0 )//���ѧϰ����Ϊ0�����˻�Ϊ�������
		{
			for( x = 0; x < cols; x++, mptr += K )
			{
				if(GmmMask[x]||nomask)
				{



					float wsum = 0;
					float pix = src[x];//ÿ������
					int kHit = -1, kForeground = -1;//�Ƿ�����ģ�ͣ��Ƿ�����ǰ��

					for( k = 0; k < K; k++ )//ÿ����˹ģ��
					{
						float w = mptr[k].weight;//��ǰģ�͵�Ȩֵ
						wsum += w;//Ȩֵ�ۼ�
						if( w < FLT_EPSILON )
							break;
						float mu = mptr[k].mean;//��ǰģ�͵ľ�ֵ
						float var = mptr[k].var;//��ǰģ�͵ķ���
						float diff = pix - mu;//��ǰ������ģ�;�ֵ֮��
						//float d2 = tmpabs(diff);//ƽ��
						float d2=diff*diff;
						if( d2 < vT*var )//�Ƿ�С�ڷ����ޣ����Ƿ����ڸ�ģ��
						{
							wsum -= w;//���ƥ�䣬�������ȥ����Ϊ֮��������
							float dw = alpha*(1.f - w);
							mptr[k].weight = w + dw;//����Ȩֵ
							//ע��Դ�������漰���ʵĲ��ֶ�����˼򻯣������ʱ�Ϊ1
							mptr[k].mean = mu + alpha*diff;//������ֵ		
							var = max(var + alpha*(d2 - var), minVar);//��ʼʱ��������0����������ʹ������������ΪĬ�Ϸ������ʹ����һ�η���
							mptr[k].var = var;//��������
							mptr[k].sortKey = w/sqrt(var);//���¼������ȼ���ò�����ﲻ�ԣ�Ӧ��ʹ�ø��º��mptr[k].weight������w

							for( k1 = k-1; k1 >= 0; k1-- )//��ƥ��ĵ�k��ģ�Ϳ�ʼ��ǰ�Ƚϣ�������º�ĵ���˹ģ�����ȼ�������ǰ����Ǹ����򽻻�˳��
							{
								if( mptr[k1].sortKey >= mptr[k1+1].sortKey )//������ȼ�û�з����ı䣬��ֹͣ�Ƚ�
									break;
								std::swap( mptr[k1], mptr[k1+1] );//�������ǵ�˳��ʼ�ձ�֤���ȼ�������ǰ��
							}

							kHit = k1+1;//��¼�����ĸ�ģ��
							break;
						}
					}

					if( kHit < 0 ) // no appropriate gaussian mixture found at all, remove the weakest mixture and create a new one
						//��ǰ���ز������κ�һ��ģ��
					{
						//��ʼ��һ����ģ��
						kHit = k = min(k, K-1);//��������������ʼ�ĳ�ʼ��ʱ��k�����ǵ���K-1��
						wsum += w0 - mptr[k].weight;//��Ȩֵ�ܺ��м�ȥԭ�����Ǹ�ģ�ͣ�������Ĭ��Ȩֵ
						mptr[k].weight = w0;//��ʼ��Ȩֵ
						mptr[k].mean = pix;//��ʼ����ֵ
						mptr[k].var = var0;	//��ʼ������
						mptr[k].sortKey = sk0;//��ʼ��Ȩֵ
					}
					else
						for( ; k < K; k++ )
							wsum += mptr[k].weight;//���ʣ�µ���Ȩֵ

					float wscale = 1.f/wsum;//��һ��
					wsum = 0;
					for( k = 0; k < K; k++ )
					{
						wsum += mptr[k].weight *= wscale;
						mptr[k].sortKey *= wscale;//�����һ��Ȩֵ
						if( wsum > T && kForeground < 0 )
							kForeground = k+1;//�ڼ���ģ��֮�����Ϊǰ����
					}

					dst[x] = (uchar)(-(kHit >= kForeground));//�о���(ucahr)(-true) = 255;(uchar)(-(false)) = 0;
				}
				else
					dst[x]=0;
			}
		}
		else//���ѧϰ����С�ڵ���0����û�б������¹��̣�������������
		{
			for( x = 0; x < cols; x++, mptr += K )
			{
				if(GmmMask[x]||nomask)
				{


				float pix = src[x];
				int kHit = -1, kForeground = -1;

				for( k = 0; k < K; k++ )
				{
					if( mptr[k].weight < FLT_EPSILON )
						break;
					float mu = mptr[k].mean;
					float var = mptr[k].var;
					float diff = pix - mu;
					float d2 = diff*diff;
					if( d2 < vT*var )
					{
						kHit = k;
						break;
					}
				}

				if( kHit >= 0 )
				{
					float wsum = 0;
					for( k = 0; k < K; k++ )
					{
						wsum += mptr[k].weight;
						if( wsum > T )
						{
							kForeground = k+1;
							break;
						}
					}
				}

				dst[x] = (uchar)(kHit < 0 || kHit >= kForeground ? 255 : 0);
			}
			}
		}
	}
}


static void process8uC3( const Mat& image, Mat& fgmask, double learningRate,
	Mat& bgmodel, int nmixtures, double backgroundRatio,
	double varThreshold, double noiseSigma )
{
	int x, y, k, k1, rows = image.rows, cols = image.cols;
	float alpha = (float)learningRate, T = (float)backgroundRatio, vT = (float)varThreshold;
	int K = nmixtures;

	const float w0 = (float)defaultInitialWeight;
	const float sk0 = (float)(w0/(defaultNoiseSigma*2*sqrt(3.)));
	const float var0 = (float)(defaultNoiseSigma*defaultNoiseSigma*4);
	const float minVar = (float)(noiseSigma*noiseSigma);
	MixData<Vec3f>* mptr = (MixData<Vec3f>*)bgmodel.data;

	for( y = 0; y < rows; y++ )
	{
		const uchar* src = image.ptr<uchar>(y);
		uchar* dst = fgmask.ptr<uchar>(y);

		if( alpha > 0 )
		{
			for( x = 0; x < cols; x++, mptr += K )
			{
				float wsum = 0;
				Vec3f pix(src[x*3], src[x*3+1], src[x*3+2]);
				int kHit = -1, kForeground = -1;

				for( k = 0; k < K; k++ )
				{
					float w = mptr[k].weight;
					wsum += w;
					if( w < FLT_EPSILON )
						break;
					Vec3f mu = mptr[k].mean;
					Vec3f var = mptr[k].var;
					Vec3f diff = pix - mu;
					float d2 = diff.dot(diff);
					if( d2 < vT*(var[0] + var[1] + var[2]) )
					{
						wsum -= w;
						float dw = alpha*(1.f - w);
						mptr[k].weight = w + dw;
						mptr[k].mean = mu + alpha*diff;
						var = Vec3f(max(var[0] + alpha*(diff[0]*diff[0] - var[0]), minVar),
							max(var[1] + alpha*(diff[1]*diff[1] - var[1]), minVar),
							max(var[2] + alpha*(diff[2]*diff[2] - var[2]), minVar));
						mptr[k].var = var;
						mptr[k].sortKey = w/sqrt(var[0] + var[1] + var[2]);

						for( k1 = k-1; k1 >= 0; k1-- )
						{
							if( mptr[k1].sortKey >= mptr[k1+1].sortKey )
								break;
							std::swap( mptr[k1], mptr[k1+1] );
						}

						kHit = k1+1;
						break;
					}
				}

				if( kHit < 0 ) // no appropriate gaussian mixture found at all, remove the weakest mixture and create a new one
				{
					kHit = k = min(k, K-1);
					wsum += w0 - mptr[k].weight;
					mptr[k].weight = w0;
					mptr[k].mean = pix;
					mptr[k].var = Vec3f(var0, var0, var0);
					mptr[k].sortKey = sk0;
				}
				else
					for( ; k < K; k++ )
						wsum += mptr[k].weight;

				float wscale = 1.f/wsum;
				wsum = 0;
				for( k = 0; k < K; k++ )
				{
					wsum += mptr[k].weight *= wscale;
					mptr[k].sortKey *= wscale;
					if( wsum > T && kForeground < 0 )
						kForeground = k+1;
				}

				dst[x] = (uchar)(-(kHit >= kForeground));
			}
		}
		else
		{
			for( x = 0; x < cols; x++, mptr += K )
			{
				Vec3f pix(src[x*3], src[x*3+1], src[x*3+2]);
				int kHit = -1, kForeground = -1;

				for( k = 0; k < K; k++ )
				{
					if( mptr[k].weight < FLT_EPSILON )
						break;
					Vec3f mu = mptr[k].mean;
					Vec3f var = mptr[k].var;
					Vec3f diff = pix - mu;
					float d2 = diff.dot(diff);
					if( d2 < vT*(var[0] + var[1] + var[2]) )
					{
						kHit = k;
						break;
					}
				}

				if( kHit >= 0 )
				{
					float wsum = 0;
					for( k = 0; k < K; k++ )
					{
						wsum += mptr[k].weight;
						if( wsum > T )
						{
							kForeground = k+1;
							break;
						}
					}
				}

				dst[x] = (uchar)(kHit < 0 || kHit >= kForeground ? 255 : 0);
			}
		}
	}
}

void MyGMM::operator()(InputArray _image, OutputArray _fgmask, double learningRate,InputArray GMM_mask,bool nomask)
{
	Mat image = _image.getMat();
	bool needToInitialize = nframes == 0 || learningRate >= 1 || image.size() != frameSize || image.type() != frameType;//�Ƿ���Ҫ��ʼ��

	if( needToInitialize )
		initialize(image.size(), image.type());//��ʼ��

	CV_Assert( image.depth() == CV_8U );
	_fgmask.create( image.size(), CV_8U );
	Mat fgmask = _fgmask.getMat();
	Mat gmmMASK= GMM_mask.getMat();
	++nframes;
	//learningRate = learningRate >= 0 && nframes > 1 ? learningRate : 1./min( nframes, history );
//	CV_Assert(learningRate >= 0);

	if( image.type() == CV_8UC1 )//�����Ҷ�ͼ��
		process8uC1( image, fgmask, learningRate, bgmodel, nmixtures, backgroundRatio, varThreshold, noiseSigma,gmmMASK,nomask);
	else if( image.type() == CV_8UC3 )//������ɫͼ��
		process8uC3( image, fgmask, learningRate, bgmodel, nmixtures, backgroundRatio, varThreshold, noiseSigma );
	else
		CV_Error( CV_StsUnsupportedFormat, "Only 1- and 3-channel 8-bit images are supported in MyGMM" );
}

void MyGMM::GetGmmBackground(Mat& _BackgroundImage)
{

	Mat meanBackground(frameSize, CV_8UC1, Scalar::all(0));//��ͨ��
	MixData<float>* mptr = (MixData<float>*)bgmodel.data; 
	for(int row=0; row<meanBackground.rows; row++)
	{
		uchar* BkGround = meanBackground.ptr<uchar>(row);  

		for(int col=0; col<meanBackground.cols; col++,mptr += nmixtures)
		{
			BkGround[col]=mptr[0].mean;
		}
	}
	_BackgroundImage=meanBackground;
}