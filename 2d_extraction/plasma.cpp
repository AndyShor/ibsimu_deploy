#include <fstream>
#include <iostream>
#include <ibsimu.hpp>
#include <error.hpp>
#include <geometry.hpp>
#include <dxf_solid.hpp>
#include <mydxffile.hpp>
#include <epot_field.hpp>
#include <epot_efield.hpp>
#include <meshvectorfield.hpp>
#include <epot_bicgstabsolver.hpp>
#include <gtkplotter.hpp>
#include <trajectorydiagnostics.hpp>



using namespace std;


double Vpuller = -40e3; //puller voltage Volts
double h = 1e-3; // mesh size meters
double nperh = 100; // particles per mesh cell
double r0 = 10e-3; // radius of initial beam (> plasma aperture)
double Npart = r0/h*nperh; // number of particles in the beam
double J = 600.0; // beam curr density A/m2
double Te = 5.0; // electron temperature eV
double E0 = 0.5*Te; // initial energy of 
double Tt = 0.1; // beam transverse temperature eV
double Up = 5.0; // plasma potential



void simu( int *argc, char ***argv )
{
  Vec3D origin( -2e-3, 0, 0 ); // origin of geometry
  Vec3D sizereq( 80e-3, 50e-3, 0 ); // domain of the problem x=z, y =r
  Int3D size( floor(sizereq[0]/h)+1, // domain in integer vector of mesh cells
	      floor(sizereq[1]/h)+1,
	      1 );
  Geometry geom( MODE_2D, size, origin, h ); // creates 2D geometry with given size, origin and cell size

  MyDXFFile *dxffile = new MyDXFFile; // constructs dxffile object
  dxffile->set_warning_level( 1 );
  dxffile->read( "geom.dxf" ); // reads given dfx file into object

  DXFSolid *s1 = new DXFSolid( dxffile, "plasma" ); // creates solid object s1 from dxf file layer named "electrode 1"
  s1->scale( 1e-3 ); // scales the imported geometry to meters
  geom.set_solid( 7, s1 ); // sets numbered solid to s1, solids must be numbered with n>=7 (first six are domain boundaries)
  DXFSolid *s2 = new DXFSolid( dxffile, "puller" );
  s2->scale( 1e-3 );
  geom.set_solid( 8, s2 );
  // set boundary conditions on domain boundaries


  geom.set_boundary( 1, Bound(BOUND_NEUMANN, 0.0) ); // xmin
  geom.set_boundary( 2, Bound(BOUND_NEUMANN, 0.0) ); // xmax
  geom.set_boundary( 3, Bound(BOUND_NEUMANN, 0.0) ); // rmin
  geom.set_boundary( 4, Bound(BOUND_NEUMANN, 0.0) ); // rmax

  geom.set_boundary( 7, Bound(BOUND_DIRICHLET, 0.0) ); // zero voltage BC on the plasma electrode
  geom.set_boundary( 8, Bound(BOUND_DIRICHLET, Vpuller) ); // puller voltage BC on the puller

  geom.build_mesh();

  EpotField epot( geom );
  MeshScalarField scharge( geom );
  MeshVectorField bfield;
  EpotEfield efield( epot );
  field_extrpl_e extrapl[6] = { FIELD_EXTRAPOLATE, FIELD_EXTRAPOLATE,
			        FIELD_SYMMETRIC_POTENTIAL, FIELD_EXTRAPOLATE,
			        FIELD_EXTRAPOLATE, FIELD_EXTRAPOLATE };
  efield.set_extrapolation( extrapl );
  
  EpotBiCGSTABSolver solver( geom );
  InitialPlasma init_plasma( AXIS_X, 1e-3 );
  solver.set_initial_plasma( Up, &init_plasma );

  ParticleDataBase2D pdb( geom );
  bool pmirror[6] = {false, false,
		     true, false,
		     false, false};
  pdb.set_mirror( pmirror ); // set particle mirroring on domain boundaries {f,f,t,f,f,f} reflect only on ymin i.e. from crossing r=0

  for( int a = 0; a < 15; a++ ) {

    ibsimu.message(1) << "Major cycle " << a << "\n";
    ibsimu.message(1) << "-----------------------\n";

    if( a == 1 ) {
	double rhoe = pdb.get_rhosum();
	solver.set_pexp_plasma( rhoe, Te, Up );
    }

    solver.solve( epot, scharge );
    if( solver.get_iter() == 0 ) {
      ibsimu.message(1) << "No iterations, breaking major cycle\n";
      break;
    }
      
    efield.recalculate();

    pdb.clear();
    // add 2D beam of Npart beamlets of protons (q=1, m=1)current density J[A/m2] with E0 initial energy, zero parallel T, Tt transverse t
    pdb.add_2d_beam_with_energy( Npart, J, 1, 1, E0, 0, Tt, 
				 geom.origo(0), 0,
				 geom.origo(0), r0 );
    pdb.iterate_trajectories( scharge, efield, bfield );

    
    TrajectoryDiagnosticData tdata;
    vector<trajectory_diagnostic_e> diag;
    diag.push_back( DIAG_Y );
    diag.push_back( DIAG_YP );
    pdb.trajectories_at_plane( tdata, AXIS_X, geom.max(0), diag );
    Emittance emit( tdata(0).data(), tdata(1).data() );

    ofstream dataout( "emit.txt", ios_base::app );
    dataout << emit.alpha() << " "
	    << emit.beta() << " "
	    << emit.epsilon() << "\n";
    dataout.close();
  }

  GTKPlotter plotter( argc, argv );
  plotter.set_geometry( &geom );
  plotter.set_epot( &epot );
  plotter.set_particledatabase( &pdb );
  plotter.set_efield( &efield );
  plotter.set_bfield( &bfield );
  plotter.set_scharge( &scharge );
  plotter.new_geometry_plot_window();
  plotter.run();
}


int main( int argc, char **argv )
{
  remove( "emit.txt" );

  try {
    ibsimu.set_message_threshold( MSG_VERBOSE, 1 );
    ibsimu.set_thread_count( 2 );
    simu( &argc, &argv );
  } catch( Error e ) {
    e.print_error_message( ibsimu.message(0) );
  }

  return( 0 );
}
