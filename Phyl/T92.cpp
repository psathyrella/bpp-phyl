//
// File: T92.h
// Created by:  <Julien.Dutheil@univ-montp2.fr>
// Created on: Mon May 26 14:41:24 2003
//

/*
Copyright ou � ou Copr. CNRS, (16 Novembre 2004) 

Julien.Dutheil@univ-montp2.fr

Ce logiciel est un programme informatique servant � fournir des classes
pour l'analyse de donn�es phylog�n�tiques.

Ce logiciel est r�gi par la licence CeCILL soumise au droit fran�ais et
respectant les principes de diffusion des logiciels libres. Vous pouvez
utiliser, modifier et/ou redistribuer ce programme sous les conditions
de la licence CeCILL telle que diffus�e par le CEA, le CNRS et l'INRIA 
sur le site "http://www.cecill.info".

En contrepartie de l'accessibilit� au code source et des droits de copie,
de modification et de redistribution accord�s par cette licence, il n'est
offert aux utilisateurs qu'une garantie limit�e.  Pour les m�mes raisons,
seule une responsabilit� restreinte p�se sur l'auteur du programme,  le
titulaire des droits patrimoniaux et les conc�dants successifs.

A cet �gard  l'attention de l'utilisateur est attir�e sur les risques
associ�s au chargement,  � l'utilisation,  � la modification et/ou au
d�veloppement et � la reproduction du logiciel par l'utilisateur �tant 
donn� sa sp�cificit� de logiciel libre, qui peut le rendre complexe � 
manipuler et qui le r�serve donc � des d�veloppeurs et des professionnels
avertis poss�dant  des  connaissances  informatiques approfondies.  Les
utilisateurs sont donc invit�s � charger  et  tester  l'ad�quation  du
logiciel � leurs besoins dans des conditions permettant d'assurer la
s�curit� de leurs syst�mes et ou de leurs donn�es et, plus g�n�ralement, 
� l'utiliser et l'exploiter dans les m�mes conditions de s�curit�. 

Le fait que vous puissiez acc�der � cet en-t�te signifie que vous avez 
pris connaissance de la licence CeCILL, et que vous en avez accept� les
termes.
*/

/*
Copyright or � or Copr. CNRS, (November 16, 2004)

Julien.Dutheil@univ-montp2.fr

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

#include "T92.h"

// From SeqLib:
#include <Seq/SequenceContainerTools.h>

// From the STL:
#include <cmath>

// From the MTL:
//#include <mtl/mtl.h>
#include <NumCalc/MatrixTools.h>

/******************************************************************************/

T92::T92(const NucleicAlphabet * alpha, double kappa, double theta):
	NucleotideSubstitutionModel(alpha),
	AbstractSubstitutionModel(alpha)
{
	thetaConstraint = new IncludingInterval(0, 1);
	_parameters.addParameter(Parameter("kappa", kappa, &Parameter::R_PLUS));
	_parameters.addParameter(Parameter("theta", theta, thetaConstraint));
	
	// Frequences:
	_freq[0] = _freq[3] = (1. - theta) / 2.;
	_freq[1] = _freq[2] = theta /2.;

	updateMatrices();
}

/******************************************************************************/

T92::~T92() {
	delete thetaConstraint;
}

/******************************************************************************/

void T92::updateMatrices() {
	double kappa = _parameters.getParameter("kappa") -> getValue();
	double theta = _parameters.getParameter("theta") -> getValue();
	
	_generator(0, 0) = -(1. +        theta * kappa);
	_generator(1, 1) = -(1. + (1. - theta) * kappa);
	_generator(2, 2) = -(1. + (1. - theta) * kappa);
	_generator(3, 3) = -(1. +        theta * kappa);

	_generator(0, 1) = (1. - theta);
	_generator(0, 3) = (1. - theta);
	_generator(1, 0) = theta;
	_generator(1, 2) = theta;
	_generator(2, 1) = theta;
	_generator(2, 3) = theta;
	_generator(3, 0) = (1. - theta);
	_generator(3, 2) = (1. - theta);
	
	_generator(0, 2) = kappa * (1. - theta);
	_generator(1, 3) = kappa * theta;
	_generator(2, 0) = kappa * theta;
	_generator(3, 1) = kappa * (1. - theta);
	
	// Normalization:
	double r = 1. / (1. + 2. * theta * kappa - 2. * theta * theta * kappa);
	MatrixTools::scale(_generator, r);
	
	// Eigen values:
	_eigenValues[0] = 0;
	_eigenValues[1] = -r * 2.;
	_eigenValues[2] = _eigenValues[3] = -r * (1. + kappa); 
	
	// Eigen vectors:
	//TODO!!!

}
	
/******************************************************************************/

double T92::Pij_t(int i, int j, double d) const
{
	double kappa = _parameters.getParameter("kappa") -> getValue();
	double theta = _parameters.getParameter("theta") -> getValue();
	double piA, piT = piA = (1. - theta)/2.;
	double piC, piG = piC = theta/2.;
	double k = (kappa + 1.) / 2.;
	double r  = 2./ (1. + 2. * theta * kappa - 2. * theta * theta * kappa);
	double l = r * d;
	double exp1 = exp(-l);
	double exp2 = exp(-k * l);
	
	switch(i) {
		//A
		case 0 : {
			switch(j) {
				case 0 : return piA * (1. + exp1) + theta * exp2; //A
				case 1 : return piC * (1. - exp1);                //C
				case 2 : return piG * (1. + exp1) - theta * exp2; //G
				case 3 : return piT * (1. - exp1);                //T, U
			}
		} 
		//C
		case 1 : {
			switch(j) {
				case 0 : return piA * (1. - exp1);                       //A
				case 1 : return piC * (1. + exp1) + (1. - theta) * exp2; //C
				case 2 : return piG * (1. - exp1);                       //G
				case 3 : return piT * (1. + exp1) - (1. - theta) * exp2; //T, U
			}
		}
		//G
		case 2 : {
			switch(j) {
				case 0 : return piA * (1. + exp1) - (1. - theta) * exp2; //A
				case 1 : return piC * (1. - exp1);                       //C
				case 2 : return piG * (1. + exp1) + (1. - theta) * exp2; //G
				case 3 : return piT * (1. - exp1);                       //T, U
			}
		}
		//T, U
		case 3 : {
			switch(j) {
				case 0 : return piA * (1. - exp1);                //A
				case 1 : return piC * (1. + exp1) - theta * exp2; //C
				case 2 : return piG * (1. - exp1);                //G
				case 3 : return piT * (1. + exp1) + theta * exp2; //T, U
			}
		}
	}
	return 0;
}

/******************************************************************************/

double T92::dPij_dt(int i, int j, double d) const
{
	double kappa = _parameters.getParameter("kappa") -> getValue();
	double theta = _parameters.getParameter("theta") -> getValue();
	double piA, piT = piA = (1. - theta)/2.;
	double piC, piG = piC = theta/2.;
	double k = (kappa + 1.) / 2.;
	double r  = 2. / (1. + 2. * theta * kappa - 2. * theta * theta * kappa);
	double l = r * d;
	double exp1 = exp(-l);
	double exp2 = exp(-k * l);

	switch(i) {
		//A
		case 0 : {
			switch(j) {
				case 0 : return r * (piA * - exp1 + theta * -k * exp2); //A
				case 1 : return r * (piC *   exp1);                     //C
				case 2 : return r * (piG * - exp1 - theta * -k * exp2); //G
				case 3 : return r * (piT *   exp1);                     //T, U
			}
		} 
		//C
		case 1 : {
			switch(j) {
				case 0 : return r * (piA *   exp1);                            //A
				case 1 : return r * (piC * - exp1 + (1. - theta) * -k * exp2); //C
				case 2 : return r * (piG *   exp1);                            //G
				case 3 : return r * (piT * - exp1 - (1. - theta) * -k * exp2); //T, U
			}
		}
		//G
		case 2 : {
			switch(j) {
				case 0 : return r * (piA * - exp1 - (1. - theta) * -k * exp2); //A
				case 1 : return r * (piC *   exp1);                            //C
				case 2 : return r * (piG * - exp1 + (1. - theta) * -k * exp2); //G
				case 3 : return r * (piT *   exp1);                            //T, U
			}
		}
		//T, U
		case 3 : {
			switch(j) {
				case 0 : return r * (piA *   exp1);                     //A
				case 1 : return r * (piC * - exp1 - theta * -k * exp2); //C
				case 2 : return r * (piG *   exp1);                     //G
				case 3 : return r * (piT * - exp1 + theta * -k * exp2); //T, U
			}
		}
	}
	return 0;
}

/******************************************************************************/

double T92::d2Pij_dt2(int i, int j, double d) const
{
	double kappa = _parameters.getParameter("kappa") -> getValue();
	double theta = _parameters.getParameter("theta") -> getValue();
	double piA, piT = piA = (1. - theta)/2.;
	double piC, piG = piC = theta/2.;
	double k = (kappa + 1.) / 2.;
	double k2 = k * k;
	double r  = 2. / (1. + 2. * theta * kappa - 2. * theta * theta * kappa);
	double l = r * d;
	double r2 = r * r;
	double exp1 = exp(-l);
	double exp2 = exp(-k * l);

	switch(i) {
		//A
		case 0 : {
			switch(j) {
				case 0 : return r2 * (piA *   exp1 + theta * k2 * exp2); //A
				case 1 : return r2 * (piC * - exp1);                     //C
				case 2 : return r2 * (piG *   exp1 - theta * k2 * exp2); //G
				case 3 : return r2 * (piT * - exp1);                     //T, U
			}
		} 
		//C
		case 1 : {
			switch(j) {
				case 0 : return r2 * (piA * - exp1);                            //A
				case 1 : return r2 * (piC *   exp1 + (1. - theta) * k2 * exp2); //C
				case 2 : return r2 * (piG * - exp1);                            //G
				case 3 : return r2 * (piT *   exp1 - (1. - theta) * k2 * exp2); //T, U
			}
		}
		//G
		case 2 : {
			switch(j) {
				case 0 : return r2 * (piA *   exp1 - (1. - theta) * k2 * exp2); //A
				case 1 : return r2 * (piC * - exp1);                            //C
				case 2 : return r2 * (piG *   exp1 + (1. - theta) * k2 * exp2); //G
				case 3 : return r2 * (piT * - exp1);                            //T, U
			}
		}
		//T, U
		case 3 : {
			switch(j) {
				case 0 : return r2 * (piA * - exp1);                     //A
				case 1 : return r2 * (piC *   exp1 - theta * k2 * exp2); //C
				case 2 : return r2 * (piG * - exp1);                     //G
				case 3 : return r2 * (piT *   exp1 + theta * k2 * exp2); //T, U
			}
		}
	}
	return 0;
}

/******************************************************************************/

Mat T92::getPij_t(double d) const {
	Mat p(_size, _size);
	double kappa = _parameters.getParameter("kappa") -> getValue();
	double theta = _parameters.getParameter("theta") -> getValue();
	double piA, piT = piA = (1. - theta)/2.;
	double piC, piG = piC = theta/2.;
	double k = (kappa + 1.) / 2.;
	double r  = 2./ (1. + 2. * theta * kappa - 2. * theta * theta * kappa);
	double l = r * d;
	double exp1 = exp(-l);
	double exp2 = exp(-k * l);

	//A
	p(0, 0) = piA * (1. + exp1) + theta * exp2; //A
	p(0, 1) = piC * (1. - exp1);                //C
	p(0, 2) = piG * (1. + exp1) - theta * exp2; //G
	p(0, 3) = piT * (1. - exp1);                //T, U

	//C
	p(1, 0) = piA * (1. - exp1);                       //A
	p(1, 1) = piC * (1. + exp1) + (1. - theta) * exp2; //C
	p(1, 2) = piG * (1. - exp1);                       //G
	p(1, 3) = piT * (1. + exp1) - (1. - theta) * exp2; //T, U

	//G
	p(2, 0) = piA * (1. + exp1) - (1. - theta) * exp2; //A
	p(2, 1) = piC * (1. - exp1);                       //C
	p(2, 2) = piG * (1. + exp1) + (1. - theta) * exp2; //G
	p(2, 3) = piT * (1. - exp1);                       //T, U

	//T, U
	p(3, 0) = piA * (1. - exp1);                //A
	p(3, 1) = piC * (1. + exp1) - theta * exp2; //C
	p(3, 2) = piG * (1. - exp1);                //G
	p(3, 3) = piT * (1. + exp1) + theta * exp2; //T, U

	return p;
}

Mat T92::getdPij_dt(double d) const {
	Mat p(_size, _size);
	double kappa = _parameters.getParameter("kappa") -> getValue();
	double theta = _parameters.getParameter("theta") -> getValue();
	double piA, piT = piA = (1. - theta)/2.;
	double piC, piG = piC = theta/2.;
	double k = (kappa + 1.) / 2.;
	double r  = 2. / (1. + 2. * theta * kappa - 2. * theta * theta * kappa);
	double l = r * d;
	double exp1 = exp(-l);
	double exp2 = exp(-k * l);

	//A
	p(0, 0) = r * (piA * - exp1 + theta * -k * exp2); //A
	p(0, 1) = r * (piC *   exp1);                     //C
	p(0, 2) = r * (piG * - exp1 - theta * -k * exp2); //G
	p(0, 3) = r * (piT *   exp1);                     //T, U

	//C
	p(1, 0) = r * (piA *   exp1);                            //A
	p(1, 1) = r * (piC * - exp1 + (1. - theta) * -k * exp2); //C
	p(1, 2) = r * (piG *   exp1);                            //G
	p(1, 3) = r * (piT * - exp1 - (1. - theta) * -k * exp2); //T, U

	//G
	p(2, 0) = r * (piA * - exp1 - (1. - theta) * -k * exp2); //A
	p(2, 1) = r * (piC *   exp1);                            //C
	p(2, 2) = r * (piG * - exp1 + (1. - theta) * -k * exp2); //G
	p(2, 3) = r * (piT *   exp1);                            //T, U

	//T, U
	p(3, 0) = r * (piA *   exp1);                     //A
	p(3, 1) = r * (piC * - exp1 - theta * -k * exp2); //C
	p(3, 2) = r * (piG *   exp1);                     //G
	p(3, 3) = r * (piT * - exp1 + theta * -k * exp2); //T, U

	return p;
}

Mat T92::getd2Pij_dt2(double d) const {
	Mat p(_size, _size);
	double kappa = _parameters.getParameter("kappa") -> getValue();
	double theta = _parameters.getParameter("theta") -> getValue();
	double piA, piT = piA = (1. - theta)/2.;
	double piC, piG = piC = theta/2.;
	double k = (kappa + 1.) / 2.;
	double k2 = k * k;
	double r  = 2. / (1. + 2. * theta * kappa - 2. * theta * theta * kappa);
	double l = r * d;
	double r2 = r * r;
	double exp1 = exp(-l);
	double exp2 = exp(-k * l);

	//A
	p(0, 0) = r2 * (piA *   exp1 + theta * k2 * exp2); //A
	p(0, 1) = r2 * (piC * - exp1);                     //C
	p(0, 2) = r2 * (piG *   exp1 - theta * k2 * exp2); //G
	p(0, 3) = r2 * (piT * - exp1);                     //T, U

	//C
	p(1, 0) = r2 * (piA * - exp1);                            //A
	p(1, 1) = r2 * (piC *   exp1 + (1. - theta) * k2 * exp2); //C
	p(1, 2) = r2 * (piG * - exp1);                            //G
	p(1, 3) = r2 * (piT *   exp1 - (1. - theta) * k2 * exp2); //T, U

	//G
	p(2, 0) = r2 * (piA *   exp1 - (1. - theta) * k2 * exp2); //A
	p(2, 1) = r2 * (piC * - exp1);                            //C
	p(2, 2) = r2 * (piG *   exp1 + (1. - theta) * k2 * exp2); //G
	p(2, 3) = r2 * (piT * - exp1);                            //T, U

	//T, U
	p(3, 0) = r2 * (piA * - exp1);                     //A
	p(3, 1) = r2 * (piC *   exp1 - theta * k2 * exp2); //C
	p(3, 2) = r2 * (piG * - exp1);                     //G
	p(3, 3) = r2 * (piT *   exp1 + theta * k2 * exp2); //T, U

	return p;
}

/******************************************************************************/

string T92::getName() const { return string("Tamura (1992)"); }

/******************************************************************************/

void T92::setFreqFromData(const SequenceContainer & data) {
	map<int, double> freqs = SequenceContainerTools::getFrequencies(data);
	double f = (freqs[1] + freqs[2]) / (freqs[0] + freqs[1] + freqs[2] + freqs[3]);
	setParameterValue("theta", f);
	_freq[0] = (1 - f) / 2;
	_freq[1] = f / 2;
	_freq[2] = f / 2;
	_freq[3] = (1 - f) / 2;
	updateMatrices();
}

/******************************************************************************/