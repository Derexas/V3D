#include <iostream>
#include <math.h>

#include <visp/vpDebug.h>
#include <visp/vpImage.h>
#include <visp/vpImageIo.h>
#include <visp/vpImageSimulator.h>
#include <visp/vpDisplayX.h>
#include <visp/vpImageConvert.h>
#include <visp/vpImageFilter.h>
#include <visp/vpMatrix.h>


using namespace std ;

void display(vpImage<unsigned char> Iout)
{
	vpDisplayX d1(Iout);
	vpDisplay::display(Iout);
	vpDisplay::flush(Iout);
	vpDisplay::getClick(Iout);
}

void display(vpImage<vpRGBa> Iout)
{
	vpDisplayX d1(Iout);
	vpDisplay::display(Iout);
	vpDisplay::flush(Iout);
	vpDisplay::getClick(Iout);
}

int blah(vpColVector col)
{
	vpRGBa color(col);
	return abs(color.R) + abs(color.G) + abs(color.B) + abs(color.A);
}

//int ssd(vpRGBa& c1, vpRGBa& c2, vpMatrix k)
int ssd(double c1, double c2)
{	
	double _sd = c1 - c2;
	int ssd = 0;
	ssd += _sd * _sd;
	return ssd;
}

int u_delta(double color, vpImage<double>& i2, int i, int j)
{
	int minimum = 1000, min_j = -1, v = 0;
	for (int jj = 0; jj < i2.getWidth(); jj++) {
//		v = blah(color, i2[i][jj]);
		v = ssd(color, i2[i][jj]);
		//cout << v << " " << jj << " " << j << endl;
		if (v < minimum) {
			minimum = v;
			min_j = jj;
		}
	}
	if (min_j < 0)
		return 0;
	return min_j - j;
}

// look on the (horizontal) epipolar line
void EWTA(vpImage<double>& i1, vpImage<double>& i2, vpImage<float>& out)
{
	for (int j = 0; j < i1.getWidth(); j++) {
		for (int i = 0; i < i1.getHeight(); i++) {
			//cout << i << " " << j << endl;
			out[i][j] = u_delta(i1[i][j], i2, i, j);
		}
	}
}

double w_ssd(vpImage<double>& i1, vpImage<double>& i2, vpImage<double>& k, int i, int j, int jj)
{
	double ssd = 0;
	int ksize = k.getHeight();
	int ks2 = ksize / 2;
	for(int u = 0; u < ksize; u++) {
		for(int v = 0; v < ksize; v++) {
			int iu = i-ks2+u;
			int jv = j-ks2+v;
			int jjv = jj-ks2+v;
			if (iu >= 0 && iu < i1.getHeight() && jv >= 0 && jv < i1.getWidth() && jjv >= 0 && jjv < i1.getHeight()) {
				int sd = i1[iu][jv] - i2[iu][jjv];
				ssd += k[u][v]*(sd*sd);
			}
		}
	}
	return ssd;
}

int w_delta(vpImage<double>& i1, vpImage<double>& i2, vpImage<double>& k, int i, int j)
{
	int minimum = 1000, min_j = -1, v = 0;
	for (int jj = 0; jj < i2.getWidth(); jj++) {
		v = w_ssd(i1, i2, k, i, j, jj);
		if (v < minimum) {
			minimum = v;
			min_j = jj;
		}
	}
	if (min_j < 0)
		return 0;
	return min_j - j;
}

void w_EWTA(vpImage<double>& i1, vpImage<double>& i2, vpImage<double>& k, vpImage<float>& out)
{
	for (int j = 0; j < i1.getWidth(); j++) {
		for (int i = 0; i < i1.getHeight(); i++) {
			out[i][j] = w_delta(i1, i2, k, i, j);
			if (j == i1.getWidth()-1) out[i][j] = 255;
		}
	}
}

void gaussianKernel(const int & taille, vpImage<double>& kernel){

  int demi_taille=(taille+1)/2;

  double filter[demi_taille];

  vpImageFilter::getGaussianKernel(filter, taille);

  for(int i=0; i<taille; i++){
    for(int j=0; j<taille; j++){
      int ii=i-taille/2, jj=j-taille/2;
      kernel[i][j]=filter[abs(ii)]*filter[abs(jj)];
      cout << kernel[i][j] << endl;
    }
  }
}

int main()
{
	int w = 384, h = 288;
	vpImage<unsigned char> IimageL(h,w);
	vpImage<unsigned char> IimageR(h,w);
	vpImage<float> Iout(h,w);
	vpImage<unsigned char> Idisplay(h,w);
	vpImage<double> IimageLO(h,w);
	vpImage<double> IimageRO(h,w);
 
	vpImageIo::read(IimageL,"../data/scene_l.pgm");
	vpImageIo::read(IimageR,"../data/scene_r.pgm");

	display(IimageL);

	int size = 7;
	/*vpImageFilter::gaussianBlur(IimageL, IimageLO, size);
	vpImageFilter::gaussianBlur(IimageR, IimageRO, size);
	EWTA(IimageLO, IimageRO, Iout);
	vpImageConvert::convert(Iout, Idisplay);
	display(Idisplay);*/

	vpImage<double> k(size, size);
	gaussianKernel(size, k);
	vpImageConvert::convert(IimageL, IimageLO);
	vpImageConvert::convert(IimageR, IimageRO);
	w_EWTA(IimageLO, IimageRO, k, Iout);
	vpImageConvert::convert(Iout, Idisplay);
	display(Idisplay);

	return 0;
}

