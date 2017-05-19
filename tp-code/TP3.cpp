#include <iostream>

#include <visp/vpDebug.h>
#include <visp/vpImage.h>
#include <visp/vpImageIo.h>
#include <visp/vpImageSimulator.h>
#include <visp/vpDisplayX.h>
#include <visp/vpDot.h>


using namespace std ;

// Calculer l'homographie aHb a partir des coordonnees des point p1 et p2
void DLT(unsigned int n,
	 vpImagePoint* p1,
	 vpImagePoint* p2,
	 vpMatrix &H21)
{
	H21 = vpMatrix(3,3);
	
	// NBPTMIN points son necessaire ; remplace le 1e6 par ce nombre
	#define NBPTMIN 4
	if(n < NBPTMIN )
	{
		cout << "there must be at least " << NBPTMIN <<	" points in the both images\n" <<endl	;
		throw ;
	}
	
	vpMatrix A(2*n,9);
	for (int i = 0; i < n; i++) {
		// Matrix A
		A[2*i][0] = A[2*i][1] = A[2*i][2] = 0;
		A[2*i][3] = -1*p1[i].get_u(); A[2*i][4] = -1*p1[i].get_v(); A[2*i][5] = -1*1;
		A[2*i][6] = p2[i].get_v()*p1[i].get_u(); A[2*i][7] = p2[i].get_v()*p1[i].get_v(); A[2*i][8] = p2[i].get_v()*1;
		A[2*i+1][0] = 1*p1[i].get_u(); A[2*i+1][1] = 1*p1[i].get_v(); A[2*i+1][2] = 1*1;
		A[2*i+1][3] = A[2*i+1][4] = A[2*i+1][5] = 0;
		A[2*i+1][6] = -p2[i].get_u()*p1[i].get_u(); A[2*i+1][7] = -p2[i].get_u()*p1[i].get_v(); A[2*i+1][8] = -p2[i].get_u()*1;
	}
	std::cout << "A\n" << A << std::endl;

	vpColVector D;
	vpMatrix U, V;

	A.svd(D, V);
	U = A;

	double minind = D[0]; int miniind = 0;
	for (int i = 1; i < D.size(); i++)
		if (D[i] < minind) {
			minind = D[i];
			miniind = i;
		}
	vpColVector h = V.getCol(miniind);
	cout << "V\n" << V << endl;
	cout << "D\n" << D << endl;
	for (int i = 0; i < h.size(); i++) {
		H21[i/3][i%3] = h[i];
	}
	cout << "H21\n" << H21 << endl;
	
}


int main()
{
	vpImage<unsigned char> I1;
	vpImage<unsigned char> I2;
	vpImage<vpRGBa> Iimage(876,1200);
	
 
	vpImageIo::read(I1,"../data/I1.pgm") ;
	vpImageIo::read(I2,"../data/I2.pgm") ;



	vpCameraParameters cam(800.0, 800.0, 200, 150);
	cam.printParameters() ;

	vpDisplayX d1(I1,10,10,"I1") ;
	vpDisplay::display(I1) ;
	vpDisplay::flush(I1) ;

	vpDisplayX d2(I2,450,10,"I2") ;
	vpDisplay::display(I2) ;
	vpDisplay::flush(I2) ;

	int nb = 5;
	vpImagePoint p1[nb], p2[nb];
	
	// clicker 5 point sur l'image I2 ; recuperer leur coordonnees
	for(unsigned int i=0; i<nb; i++)
	{
		vpDisplay::getClick(I1, p1[i]) ;
		vpDot d ;
		d.initTracking(I1,p1[i]) ;
		d.track(I1,p1[i]) ;
		char s[10] ;
		sprintf(s,"%d",i) ;
		vpDisplay::displayCross(I1,p1[i],10,vpColor::blue) ;
		vpDisplay::displayCharString(I1,p1[i],s,vpColor::red) ;
		vpDisplay::flush(I1) ;
	}
	
	// clicker 5 point sur l'image I1 ; recuperer leur coordonnees
	// faites attention a les clicker dans le meme ordre
	for(unsigned int i=0; i<nb; i++)
	{
		vpDisplay::getClick(I2, p2[i]) ;
		vpDot d ;
		d.initTracking(I2,p2[i]) ;
		d.track(I2,p2[i]) ;
		char s[10] ;
		sprintf(s,"%d",i) ;
		vpDisplay::displayCross(I2,p2[i],10,vpColor::green) ;
		vpDisplay::displayCharString(I2,p2[i],s,vpColor::red) ;
		vpDisplay::flush(I2) ;
	}
	

	// Calculer l'homographie
	vpMatrix H21 ;
	DLT(nb, p1, p2, H21) ;

	cout << "Homographie H21 : " << endl ; 
	cout << H21 << endl ;

	//Verification 
	double residue = 0;
	for (int i=0 ; i < nb ; i++) 
	{
		// Connaissant le formule permettant le transfert des points p2 dans p1
		// Calculer les coordonnées des point p1 connaissant p2 et dHg
		vpColVector v1(3), v2(3);
		v1[0] = p1[i].get_u(); v1[1] = p1[i].get_v(); v1[2] = 1;
		v2 = H21 * v1; v2 /= v2[2];
		vpImagePoint p2_calcule; p2_calcule.set_uv(v2[0], v2[1]);

		// en deduire l'erreur sur commise sur chaque point et 
		// afficher un cercle de rayon 10 fois cette erreur
		double r ;
		r = vpImagePoint::distance(p2_calcule, p2[i]);
		cout << "point " << i << "	" << r << " " << p2_calcule << " " << p2[i] << endl ;
		double rayon ;
		rayon = sqrt(r)*10 ; if (rayon < 10) rayon = 10 ;
		vpDisplay::displayCircle(I2, p2_calcule, rayon, vpColor::green);
	}

	vpDisplay::flush(I2) ;
	vpImage<vpRGBa> Ic ;
	vpDisplay::getImage(I2,Ic) ;
	vpImageIo::write(Ic,"resultat.jpg") ;

	vpDisplay::getClick(I2) ;

	vpDisplay::close(I2) ;
	vpDisplay::close(I1) ;

	return 0;
}
