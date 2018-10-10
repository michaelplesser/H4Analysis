#include "TrackReco.h"
#include "Math/Interpolator.h"
#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"

double TrackReco::Track::chi2(const double* par)
{
  if (par) //set the actual fit parameters
    {
      position_(0)=par[0];
      position_(1)=par[1];
      angle_(0)=par[2];
      angle_(1)=par[3];
    }

   double chi2=0;
   for (auto& hit: TrackReco::Track::hits_)
     {
       //calculate the residual
       Measurement_t pos;
       MeasurementErrorMatrix_t posErrInverse;
       hit.globalPosition(pos);
       hit.globalPositionErrorInverse(posErrInverse);
       Measurement_t delta= pos-this->statusAt(this->hodo_.layers_[hit.layer_].position_(3)); //calculation of the residual
       chi2+= ROOT::Math::Dot(delta,posErrInverse*delta); // delta^T * (V^-1) * delta
     }
   return chi2;
}

bool TrackReco::Track::fitTrack()
{
  //---setup minimization
  ROOT::Math::Functor chi2(this, &TrackReco::Track::chi2, 4);
  ROOT::Math::Minimizer* minimizer = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");
  minimizer->SetMaxFunctionCalls(100000);
  minimizer->SetMaxIterations(1000);
  minimizer->SetTolerance(1e-3);
  minimizer->SetPrintLevel(0);
  minimizer->SetFunction(chi2);
  minimizer->SetLimitedVariable(0, "X",0, 1E-2, -30, 30);
  minimizer->SetLimitedVariable(1, "Y",0, 1E-2, -30, 30);
  minimizer->SetLimitedVariable(2, "alpha",0, 1E-4,  -0.1, 0.1);
  minimizer->SetLimitedVariable(3, "beta", 0, 1E-4,  -0.1, 0.1);

  //---fit
  minimizer->Minimize();
  position_(0) = minimizer->X()[0];
  position_(1) = minimizer->X()[1];
  angle_(0) = minimizer->X()[2];
  angle_(1) = minimizer->X()[3];
  
  delete minimizer;        

  return true;
}

//----------Begin-------------------------------------------------------------------------
bool TrackReco::Begin(CfgManager& opts, uint64* index)
{
    //---create a position tree
//    bool storeTree = opts.OptExist(instanceName_+".storeTree") ?
//        opts.GetOpt<bool>(instanceName_+".storeTree") : true;

    return true;
}

//----------ProcessEvent------------------------------------------------------------------
bool TrackReco::ProcessEvent(H4Tree& h4Tree, map<string, PluginBase*>& plugins, CfgManager& opts)
{

    tracks_.clear();

    GlobalCoord_t layer0Pos(0,0,0);
    GlobalCoord_t layer1Pos(0,0,100);
    TrackLayer layer_0(layer0Pos);
    TrackLayer layer_1(layer0Pos);
    TelescopeLayout hodo;
    hodo.addLayer(layer_0);
    hodo.addLayer(layer_1);
    TrackMeasurement aHit_0(0,0,hodo,0);
    aHit_0.setVarianceX(1);
    aHit_0.setVarianceY(1);
    aHit_0.calculateInverseVariance();
    TrackMeasurement aHit_1(0,0,hodo,0);
    aHit_1.setVarianceX(1);
    aHit_1.setVarianceY(1);
    aHit_1.calculateInverseVariance();
    Track aTrack(hodo);
    aTrack.addMeasurement(aHit_0);
    aTrack.addMeasurement(aHit_1);

    aTrack.fitTrack();

    //---fill output tree
    return true;
}
