//
// File: PartitionProcessPhyloLikelihood.cpp
// Created by: Laurent Guéguen
// Created on: samedi 16 mai 2015, à 13h 54
//

/*
   Copyright or © or Copr. Bio++ Development Team, (November 16, 2004)

   This software is a computer program whose purpose is to provide classes
   for phylogenetic data analysis.

   This software is governed by the CeCILL  license under French law and
   abiding by the rules of distribution of free software.  You can  use,
   modify and/ or redistribute the software under the terms of the CeCILL
   license as circulated by CEA, CNRS and INRIA at the following URL
   "http://www.cecill.info".

   As a counterpart to the access to the source code and  rights to copy,
   modify and redistribute granted by the license, users are provided only
   with a limited warranty  and the software's author,  the holder of the
   economic rights,  and the successive licensors  have only  limited
   liability.

   In this respect, the user's attention is drawn to the risks associated
   with loading,  using,  modifying and/or developing or reproducing the
   software by the user in light of its specific status of free software,
   that may mean  that it is complicated to manipulate,  and  that  also
   therefore means  that it is reserved for developers  and  experienced
   professionals having in-depth computer knowledge. Users are therefore
   encouraged to load and test the software's suitability as regards their
   requirements in conditions enabling the security of their systems and/or
   data to be ensured and,  more generally, to use and operate it in the
   same conditions as regards security.

   The fact that you are presently reading this means that you have had
   knowledge of the CeCILL license and that you accept its terms.
 */

#include "PartitionProcessPhyloLikelihood.h"

#include "SingleProcessPhyloLikelihood.h"

#include "PhyloLikelihoodContainer.h"

#include <Bpp/Seq/Container/SiteContainerTools.h>

using namespace std;
using namespace bpp;

/******************************************************************************/

PartitionProcessPhyloLikelihood::PartitionProcessPhyloLikelihood(
  Context& context,
  PartitionSequenceEvolution& processSeqEvol,
  size_t nSeqEvol,
  bool verbose,
  bool patterns) :
  AbstractPhyloLikelihood(context),
  AbstractAlignedPhyloLikelihood(context, 0),
  SequencePhyloLikelihood(context, processSeqEvol, nSeqEvol),
  ProductOfAlignedPhyloLikelihood(context, new PhyloLikelihoodContainer()),
  mSeqEvol_(processSeqEvol),
  vProcPos_()
{
  SubstitutionProcessCollection& processColl = processSeqEvol.getCollection();
  map<size_t, vector<size_t> >& mProcPos=processSeqEvol.getMapOfProcessSites();

  vProcPos_.resize(processSeqEvol.getNumberOfSites());

  PhyloLikelihoodContainer* pC=getPhyloContainer();
  
  for (std::map<size_t, std::vector<size_t> >::iterator it=mProcPos.begin(); it!=mProcPos.end(); it++)
  {
    auto nProcess = it->first;
    
    auto l = std::make_shared<LikelihoodCalculationSingleProcess>(context, processColl.getSubstitutionProcess(nProcess));

    auto nPL = make_shared<SingleProcessPhyloLikelihood>(context, l, nProcess);

    pC->sharePhyloLikelihood(nProcess, nPL);

    for (size_t i = 0; i<it->second.size();i++)
    {
      vProcPos_[it->second[i]].nProc=nProcess;
      vProcPos_[it->second[i]].pos=i;      
    }
  }

  addAllPhyloLikelihoods(false);
}


/******************************************************************************/

PartitionProcessPhyloLikelihood::PartitionProcessPhyloLikelihood(
  Context& context,
  const AlignedValuesContainer& data,
  PartitionSequenceEvolution& processSeqEvol,
  size_t nSeqEvol,
  size_t nData,
  bool verbose,
  bool patterns) :
  AbstractPhyloLikelihood(context),
  AbstractAlignedPhyloLikelihood(context, data.getNumberOfSites()),
  SequencePhyloLikelihood(context, processSeqEvol, nSeqEvol, nData),
  ProductOfAlignedPhyloLikelihood(context, new PhyloLikelihoodContainer()),
  mSeqEvol_(processSeqEvol),
  vProcPos_()
{
  if (data.getNumberOfSites()!=processSeqEvol.getNumberOfSites())
    throw BadIntegerException("PartitionProcessPhyloLikelihood::PartitionProcessPhyloLikelihood, data and sequence process lengths do not match.", (int)data.getNumberOfSites());

  SubstitutionProcessCollection& processColl = processSeqEvol.getCollection();
  map<size_t, vector<size_t> >& mProcPos=processSeqEvol.getMapOfProcessSites();

  vProcPos_.resize(processSeqEvol.getNumberOfSites());

  PhyloLikelihoodContainer* pC=getPhyloContainer();
  
  for (std::map<size_t, std::vector<size_t> >::iterator it=mProcPos.begin(); it!=mProcPos.end(); it++)
  {
    auto nProcess = it->first;
    
    auto l = std::make_shared<LikelihoodCalculationSingleProcess>(context, processColl.getSubstitutionProcess(nProcess));

    auto nPL = make_shared<SingleProcessPhyloLikelihood>(context, l, nProcess, nData);

    pC->sharePhyloLikelihood(nProcess, nPL);

    for (size_t i = 0; i<it->second.size();i++)
    {
      vProcPos_[it->second[i]].nProc=nProcess;
      vProcPos_[it->second[i]].pos=i;      
    }
  }

  addAllPhyloLikelihoods(false);
  
  setData(data, nData);
}

/******************************************************************************/

bool PartitionProcessPhyloLikelihood::addPhyloLikelihood(size_t nPhyl)
{
  const AbstractAlignedPhyloLikelihood* aPL=getAbstractPhyloLikelihood(nPhyl);
  
  if (aPL!=NULL)
  {
    nPhylo_.push_back(nPhyl);
    includeParameters_(aPL->getParameters());
    return true;
  }

  return false;
}

/******************************************************************************/

void PartitionProcessPhyloLikelihood::setData(const AlignedValuesContainer& data, size_t nData)
{
  if (data.getNumberOfSites()!=mSeqEvol_.getNumberOfSites())
    throw BadIntegerException("PartitionProcessPhyloLikelihood::PartitionProcessPhyloLikelihood, data and sequence process lengths do not match.", (int)data.getNumberOfSites());

  SequencePhyloLikelihood::setData(data, nData);

  const map<size_t, vector<size_t> >& mProcPos=mSeqEvol_.getMapOfProcessSites();

  for (std::map<size_t, std::vector<size_t> >::const_iterator it=mProcPos.begin(); it!=mProcPos.end(); it++)
  {
    auto st=shared_ptr<AlignedValuesContainer>(SiteContainerTools::getSelectedSites(data, it->second));
    getPhyloContainer()->setData(*st, it->first);    
  }
}

/******************************************************************************/

double PartitionProcessPhyloLikelihood::getLikelihoodForASite(size_t site) const
{
  return getPhyloLikelihood(vProcPos_[site].nProc)->getLikelihoodForASite(vProcPos_[site].pos);
}

/******************************************************************************/

double PartitionProcessPhyloLikelihood::getLogLikelihoodForASite(size_t site) const
{
  return getPhyloLikelihood(vProcPos_[site].nProc)->getLogLikelihoodForASite(vProcPos_[site].pos);
}

/******************************************************************************/

// double PartitionProcessPhyloLikelihood::getDLogLikelihoodForASite(const std::string& variable, size_t site) const
// {
//   return getPhyloLikelihood(vProcPos_[site].nProc)->getDLogLikelihoodForASite(variable, vProcPos_[site].pos);
// }

// /******************************************************************************/

// double PartitionProcessPhyloLikelihood::getD2LogLikelihoodForASite(const std::string& variable, size_t site) const
// {
//   return getPhyloLikelihood(vProcPos_[site].nProc)->getD2LogLikelihoodForASite(variable, vProcPos_[site].pos);
// }

