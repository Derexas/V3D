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

	display(IimageL); cout << "a" << endl;

	int size = 3;
	vpImageFilter::gaussianBlur(IimageL, IimageLO, size);
	vpImageFilter::gaussianBlur(IimageR, IimageRO, size);
	EWTA(IimageLO, IimageRO, Iout); cout << "d" << endl;
	vpImageConvert::convert(Iout, Idisplay); cout << "d" << endl;
	display(Idisplay);

	//vpImageIo::write(Icamera,"I1g.jpg");*/

	return 0;
}

