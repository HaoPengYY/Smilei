#ifndef ELECTROMAGN_H
#define ELECTROMAGN_H

#include <vector>
#include <string>
#include <map>

#include "Field.h"
#include "Tools.h"
#include "Profile.h"
#include "Species.h"


class Params;
class Projector;
class Laser;
class ElectroMagnBC;
class SimWindow;
class Patch;
class Solver;


// ---------------------------------------------------------------------------------------------------------------------
//! This structure contains the properties of each ExtField
// ---------------------------------------------------------------------------------------------------------------------
struct ExtField {
    //! field to which apply the external field
    std::string field;
    
    Profile * profile;
};

// ---------------------------------------------------------------------------------------------------------------------
//! This structure contains the properties of each Antenna
// ---------------------------------------------------------------------------------------------------------------------
struct Antenna {
    //! Jx, Jy or Jz
    std::string fieldName;
    
    Profile *time_profile;
    Profile *space_profile;
    
    Field* field;
};

//! class ElectroMagn: generic class containing all information on the electromagnetic fields and currents
class ElectroMagn
{

public:
    //! Constructor for Electromagn
    ElectroMagn( Params &params, std::vector<Species*>& vecSpecies, Patch* patch );
    ElectroMagn( ElectroMagn* emFields, Params &params, Patch* patch );
    void initElectroMagnQuantities();
    //! Extra initialization. Used in ElectroMagnFactory
    void finishInitialization(int nspecies, Patch* patch);
    
    //! Destructor for Electromagn
    virtual ~ElectroMagn();

    void updateGridSize(Params &params, Patch* patch);

    void clean();
        
    std::vector<unsigned int> dimPrim;
    std::vector<unsigned int> dimDual;
    
    std::vector<unsigned int> index_bc_min;
    std::vector<unsigned int> index_bc_max;
    
    //! time-step (from Params)
    const double timestep;
    
    //! cell length (from Params)
    const std::vector<double> cell_length;
    
    //! \todo Generalise this to none-cartersian geometry (e.g rz, MG & JD)
    
    //! x-component of the electric field
    Field* Ex_;
    
    //! y-component of the electric field
    Field* Ey_;
    
    //! z-component of the electric field
    Field* Ez_;
    
    //! x-component of the magnetic field
    Field* Bx_;
    
    //! y-component of the magnetic field
    Field* By_;
    
    //! z-component of the magnetic field
    Field* Bz_;
    
    //! x-component of the time-centered magnetic field
    Field* Bx_m;
    
    //! y-component of the time-centered magnetic field
    Field* By_m;
    
    //! z-component of the time-centered magnetic field
    Field* Bz_m;
    
    //! x-component of the total charge current
    Field* Jx_;
    
    //! y-component of the total charge current
    Field* Jy_;
    
    //! z-component of the total charge current
    Field* Jz_;
    
    //! Total charge density
    Field* rho_;
    
    //! Vector of electric fields used when a filter is applied
    std::vector<Field*> Exfilter;
    std::vector<Field*> Eyfilter;
    std::vector<Field*> Ezfilter;
    
    //! Vector of magnetic fields used when a filter is applied
    std::vector<Field*> Bxfilter;
    std::vector<Field*> Byfilter;
    std::vector<Field*> Bzfilter;

    
    //! all Fields in electromagn (filled in ElectromagnFactory.h)
    std::vector<Field*> allFields;
    
    //! all Fields averages required in diagnostic Fields
    std::vector<std::vector<Field*> > allFields_avg;
    
    //! Vector of charge density and currents for each species
    const unsigned int n_species;
    std::vector<Field*> Jx_s;
    std::vector<Field*> Jy_s;
    std::vector<Field*> Jz_s;
    std::vector<Field*> rho_s;
    
    //! Creates a new field with the right characteristics, depending on the name
    virtual Field * createField(std::string fieldname) = 0;
    
    //! Number of bins
    unsigned int nbin;
    //! Cluster width
    unsigned int clrw;
    
    //! nDim_field (from params)
    const unsigned int nDim_field;
    
    //! Volume of the single cell (from params)
    const double cell_volume;
    
    //! n_space (from params) always 3D
    const std::vector<unsigned int> n_space;
    
    //! Index of starting elements in arrays without duplicated borders
    //! By constuction 1 element is shared in primal field, 2 in dual
    //! 3 : Number of direction (=1, if dim not defined)
    //! 2 : isPrim/isDual
    unsigned int istart[3][2];
    //! Number of elements in arrays without duplicated borders
    unsigned int bufsize[3][2];
    
    //!\todo should this be just an integer???
    //! Oversize domain to exchange less particles (from params)
    const std::vector<unsigned int> oversize;
    
    //! Constructor for Electromagn
    ElectroMagn( Params &params, Patch* patch );
    
    //! Method used to initialize the total charge currents and densities
    void restartRhoJ();
    //! Method used to initialize the total charge currents and densities of species
    void restartRhoJs();
    
    //! Method used to sum all species densities and currents to compute the total charge density and currents
    virtual void computeTotalRhoJ() = 0;
    
    virtual void initPoisson(Patch *patch) = 0;
    virtual double compute_r() = 0;
    virtual void compute_Ap(Patch *patch) = 0;
    //Access to Ap
    virtual double compute_pAp() = 0;
    virtual void update_pand_r(double r_dot_r, double p_dot_Ap) = 0;
    virtual void update_p(double rnew_dot_rnew, double r_dot_r) = 0;
    virtual void initE(Patch *patch) = 0;
    virtual void centeringE( std::vector<double> E_Add ) = 0;
    
    virtual double getEx_Xmin() = 0; // 2D !!!
    virtual double getEx_Xmax() = 0; // 2D !!!
    
    virtual double getEx_XminYmax() = 0; // 1D !!!
    virtual double getEy_XminYmax() = 0; // 1D !!!
    virtual double getEx_XmaxYmin() = 0; // 1D !!!
    virtual double getEy_XmaxYmin() = 0; // 1D !!!
    
    std::vector<unsigned int> index_min_p_;
    std::vector<unsigned int> index_max_p_;
    Field* phi_;
    Field* r_;
    Field* p_;
    Field* Ap_;
    
    //! \todo check time_dual or time_prim (MG)
//    //! method used to solve Maxwell's equation (takes current time and time-step as input parameter)
//    virtual void solveMaxwellAmpere() = 0;
    //! Maxwell Ampere Solver
    Solver* MaxwellAmpereSolver_;
    //! Maxwell Faraday Solver
    Solver* MaxwellFaradaySolver_;
    virtual void saveMagneticFields() = 0;
    virtual void centerMagneticFields() = 0;
    virtual void binomialCurrentFilter() = 0;
    
    void boundaryConditions(int itime, double time_dual, Patch* patch, Params &params, SimWindow* simWindow);
    
    void laserDisabled();
    
    void incrementAvgField(Field * field, Field * field_avg);
        
    //! compute Poynting on borders
    virtual void computePoynting() = 0;
    
    //! pointing vector on borders 
    //! 1D: poynting[0][0]=left , poynting[1][0]=right
    //! 2D: poynting[0][0]=xmin , poynting[1][0]=xmax
    //!     poynting[1][0]=ymin, poynting[1][0]=ymax
    std::vector<double> poynting[2];
    
    //same as above but instantaneous
    std::vector<double> poynting_inst[2];
    
    //! Compute local square norm of charge denisty is not null
    inline double computeRhoNorm2() {
        return rho_->norm2(istart, bufsize);
    }

    //! Compute local sum of Ex
    inline double computeExSum() {
        return Ex_->sum(istart, bufsize);
    }
    //! Compute local sum of Ey
    inline double computeEySum() {
        return Ey_->sum(istart, bufsize);
    }
    //! Compute local sum of Ez
    inline double computeEzSum() {
        return Ez_->sum(istart, bufsize);
    }
    
    //! external fields parameters the key string is the name of the field and the value is a vector of ExtField
    std::vector<ExtField> extFields;
    
    //! Method used to impose external fields (apply to all Fields)
    void applyExternalFields(Patch*);
    
    //! Method used to impose external fields (apply to a given Field)
    virtual void applyExternalField(Field*, Profile*, Patch*) = 0 ;
    
    //! Antenna
    std::vector<Antenna> antennas;
    
    //! Method used to impose external currents (aka antennas)
    void applyAntenna(unsigned int iAntenna, double intensity);
    
    //! Method that fills the initial spatial profile of the antenna
    virtual void initAntennas(Patch* patch) {};
    
    double computeNRJ();
    double getLostNrjMW() const {return nrj_mw_lost;}
    
    double getNewFieldsNRJ() const {return nrj_new_fields;}
    
    void reinitDiags() {
        nrj_mw_lost = 0.;
        nrj_new_fields = 0.;
    }
    
    inline void storeNRJlost( double nrj ) { nrj_mw_lost += nrj; }
    
    inline int getMemFootPrint() {
    
        int emSize = 9+4; // 3 x (E, B, Bm) + 3 x J, rho
        for (unsigned int ispec=0 ; ispec<Jx_s.size() ; ispec++) {
            if (Jx_s [ispec]) emSize++;
            if (Jy_s [ispec]) emSize++;
            if (Jz_s [ispec]) emSize++;
            if (rho_s [ispec]) emSize++;
        }

        for ( unsigned int idiag = 0 ; idiag < allFields_avg.size() ; idiag++)
            emSize += allFields_avg[idiag].size() ;


        for (size_t i=0 ; i<nDim_field ; i++)
            emSize *= dimPrim[i];
    
        emSize *= sizeof(double);
        return emSize;
    }
    
    //! Vector of boundary-condition per side for the fields
    std::vector<ElectroMagnBC*> emBoundCond;
    
protected :
    //! from smpi is xmin
    bool isXmin;
    
    //! from smpi is xmax
    bool isXmax;
    
private:
    
    //! Accumulate nrj lost with moving window
    double nrj_mw_lost;
    
    //! Accumulate nrj added with new fields
    double nrj_new_fields;
    
};

#endif
