//
// File: TS98.h
// Created by: Julien Dutheil
// Created on: Sat Aug 19 11:37 2006
//

/*
Copyright or � or Copr. CNRS, (November 16, 2004)

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

#ifndef _TS98_H_
#define _TS98_H_

#include "MarkovModulatedSubstitutionModel.h"

//From NumCalc:
#include <NumCalc/DiscreteDistribution.h>
#include <NumCalc/Parameter.h>

/**
 * @brief Tuffley and Steel's 1998 covarion model.
 *
 * This model is a subclass of the so-called Markov-modulated substitution models,
 * with a rate matrix
 * @f[
 * G = \begin{pmatrix}
 * -s_1 & s_1\\
 * s_2 & -s_2
 * \end{pmatrix}
 * @f]
 * and
 * @f[
 * D_R = \begin{pmatrix}
 * 0 & 0\\
 * 0 & \dfrac{s_1+s_2}{s_1}
 * \end{pmatrix}.
 * @f]
 * This model was originally designed for nucleotides sequences, but it can be used with other alphabets.
 *
 * @see MarkovModulatedSubstitutionModel
 *
 * Tuffley C. and Steel M. A., Modelling the covarion hypothesis of nucleotide substitution (1998),
 * _Math. Biosci._, 147:63-91.
 */
class TS98: public MarkovModulatedSubstitutionModel
{
  public:
    /**
     * @brief Build a new TS98 substitution model.
     *
     * @param model The substitution model to use. May be of any alphabet type.
     * @param s1    First rate parameter.
     * @param s2    Second rate parameter.
     */
    TS98(SubstitutionModel * model, double s1 = 1., double s2 = 1., bool normalizeRateChanges = false):
      MarkovModulatedSubstitutionModel(model, normalizeRateChanges)
    {
      _nbRates = 2;
      _ratesExchangeability.resize(2, 2);
      _rates.resize(2, 2);
      _ratesFreq.resize(2);
      _ratesParameters.addParameter(Parameter("s1", s1, &Parameter::R_PLUS_STAR));
      _ratesParameters.addParameter(Parameter("s2", s2, &Parameter::R_PLUS_STAR));
      updateRatesModel();
      updateMatrices();
    }
    virtual ~TS98() {}

  public:
    string getName() const { return "Tuffley and Steel Covarion Model (Tuffley and Steel 1998)"; }
    
  protected:
    void updateRatesModel()
    {
      double s1 = _ratesParameters.getParameter("s1")->getValue();
      double s2 = _ratesParameters.getParameter("s2")->getValue();
      _ratesFreq[0] = s2/(s1+s2);
      _ratesFreq[1] = s1/(s1+s2);
      _rates(1,1) = (s1+s2)/s1;
      _ratesExchangeability(0,1) = _ratesExchangeability(1,0) = s1+s2;
      _ratesExchangeability(0,0) = -s1*(s1+s2)/s2;
      _ratesExchangeability(1,1) = -s2*(s1+s2)/s1;
    }
	
};

#endif // _TS98_H_
