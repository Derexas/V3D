#include <iostream>

#include <visp/vpDebug.h>
#include <visp/vpImage.h>
#include <visp/vpImageIo.h>
#include <visp/vpImageSimulator.h>
#include <visp/vpDisplayX.h>


using namespace std ;

void process_x2_y2(double& x2, double& y2, double x1, double y1, double Z, 
	vpHomogeneousMatrix m)
{
	cout << m << endl;
  x2 = (Z*(m[0][0]*x1 + m[0][1]*y1 + m[0][2]*1) + m[0][3]) / (Z*(m[2][0]*x1 + m[2][1]*y1 + m[2][2]*1) + m[2][3]);
  y2 = (Z*(m[1][0]*x1 + m[1][1]*y1 + m[1][2]*1) + m[1][3]) / (Z*(m[2][0]*x1 + m[2][1]*y1 + m[2][2]*1) + m[2][3]);
}

int main()
{
  vpImage<unsigned char> Ig(300,400,0);
  vpImage<unsigned char> Id(300,400,0);
  vpImage<vpRGBa> Iimage(876,1200);
  vpImageIo::read(Iimage,"../data/big-sleep.jpg");

  double L = 0.600 ;
  double l = 0.438;
  // Initialise the 3D coordinates of the Iimage corners
  vpColVector X[4];
  for (int i = 0; i < 4; i++) X[i].resize(3);
  // Top left corner
  X[0][0] = -L;
  X[0][1] = -l;
  X[0][2] = 0;
  
  // Top right corner
  X[1][0] = L;
  X[1][1] = -l;
  X[1][2] = 0;
  
  // Bottom right corner
  X[2][0] = L;
  X[2][1] = l;
  X[2][2] = 0;
  
  //Bottom left corner
  X[3][0] = -L;
  X[3][1] = l;
  X[3][2] = 0;
  


  vpImageSimulator sim;
  sim.init(Iimage, X);
  double px = 800; double py = 800;
  int u0 = 200; int v0 = 150;
  vpCameraParameters cam(px, py, u0, v0);


  cam.printParameters() ;

  double tx = 0.1; double ty = 0.; double tz = -0.1;

  // I1g
  vpHomogeneousMatrix  gMo(tx, ty, 2+tz, vpMath::rad(5),vpMath::rad(5),vpMath::rad(5));

  sim.setCameraPosition(gMo);
  sim.getImage(Ig,cam);
  cout << "Image I1g " << endl ;
  cout << "gMo " << endl ;
  cout << gMo << endl ;

  // I1d
  vpHomogeneousMatrix  dMo(0, 0, 2, vpMath::rad(0),vpMath::rad(0),0);
//  vpHomogeneousMatrix dMo(0.1, 0, 1.9, vpMath::rad(5),vpMath::rad(5),vpMath::rad(5));
  sim.setCameraPosition(dMo);
  sim.getImage(Id,cam);
  cout << "Image I1d " << endl ;
  cout << "dMo " << endl ;
  cout << dMo << endl ;
 

  vpDisplayX dg(Ig,10,10,"Ig") ;
  vpDisplay::display(Ig) ;
  vpDisplay::flush(Ig) ;

  vpDisplayX dd(Id,10,10,"Id") ;
  vpDisplay::display(Id) ;
  vpDisplay::flush(Id) ;

	vpHomogeneousMatrix gtd = gMo*dMo.inverse();
	vpHomogeneousMatrix dtg = dMo*gMo.inverse();

  vpImagePoint pd ;

  for (int i=0 ; i < 10 ; i++)
    {
      cout << "Click point number " << i << endl ;
      vpDisplay::getClick(Id, pd) ;
      
      
      vpDisplay::displayCross(Id,pd,5,vpColor::red) ;
      //*/

      // Calcul du lieu geometrique
      //....
      double u = pd.get_u();
      double v = pd.get_v();

      double x1 = (u - u0) / px;
      double y1 = (v - v0) / py;
      double Z = 1;
      double x2, y2;

      vpImagePoint p1, p2, p3, p4;
      
	process_x2_y2(x2, y2, x1, y1, Z, gtd);
      p1.set_u(x2 * px + u0); p1.set_v(y2 * py + v0);
      Z = 1000;
      process_x2_y2(x2, y2, x1, y1, Z, gtd);
      p2.set_u(x2 * px + u0); p2.set_v(y2 * py + v0);
      std::cout << pd << " | " << p1 << " | " << p2 << std::endl;

	p3 = p1 - p2; p3.set_u(p3.get_u()*20); p3.set_v(p3.get_v()*20);
	p1 = p1 + p3; p2 = p2 - p3;

      vpDisplay::displayLine(Ig, p1, p2, vpColor::red);

      //*/// ----------------------------------------------------------

      /*vpDisplay::getClick(Ig, pd);
      vpDisplay::displayCross(Ig,pd,5,vpColor::red);
      u = pd.get_u();
      v = pd.get_v();
      x1 = (u - u0) / px;
      y1 = (v - v0) / py;
      Z = 0.1;
      process_x2_y2(x2, y2, x1, y1, Z, dtg);
      p1.set_u(x2 * px + u0); p1.set_v(y2 * py + v0);
      Z = 1000;
      process_x2_y2(x2, y2, x1, y1, Z, dtg);
      p2.set_u(x2 * px + u0); p2.set_v(y2 * py + v0);
      std::cout << pd << " | " << p1 << " | " << p2 << std::endl;

	p3 = p1 - p2; p3.set_u(p3.get_u()*20); p3.set_v(p3.get_v()*20);
	p1 = p1 + p3; p2 = p2 - p3;

      vpDisplay::displayLine(Id, p1, p2, vpColor::red);//*/

      // Affichage dans Ig
      
      //      vpDisplay::displayXXXX(Ig,...) ;

      vpDisplay::flush(Id) ;
      vpDisplay::flush(Ig) ;
    }

  // exemple de code pour sauvegarder une image avec les plan overlay
  vpImage<vpRGBa> Icol ;
  vpDisplay::getImage(Id,Icol) ;
  vpImageIo::write(Icol,"resultat.jpg") ;
  vpImageIo::write(Id,"I1g.jpg") ;

  


  vpDisplay::getClick(Id) ;
  cout << "OK " << endl ;

  vpDisplay::close(Id) ;
  vpDisplay::close(Ig) ;

  

  return 0;
}
