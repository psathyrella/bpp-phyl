//
// File: JCnuc.cpp
// Created by: Julien Dutheil
// Created on: Tue May 27 16:04:36 2003
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

#include "JCnuc.h"


#include <cmath>

/******************************************************************************/

JCnuc::JCnuc(const NucleicAlphabet * alpha): NucleotideSubstitutionModel(alpha), AbstractSubstitutionModel(alpha)
{
	_parameters = ParameterList(); //no parameters for this model.	
	
	// Frequences:
	_freq[0] = _freq[1] = _freq[2] = _freq[3] = 1. / 4.;
	
	updateMatrices();
}

JCnuc::~JCnuc() {}

/******************************************************************************/
	
void JCnuc::updateMatrices()
{
	// Generator:
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			_generator(i, j) = (i == j) ? -1. : 1./3.;
		}
	}
	
	// Eigen values:
	_eigenValues[0] = 0;
	_eigenValues[1] = _eigenValues[2] = _eigenValues[3] = -4. / 3.;
	
	// Eigen vectors:
	// todo!
}
	
/******************************************************************************/

// Generator matrix: Q =
//                            [       1    1    1  ]
//                            [ - 1   -    -    -  ]
//                            [       3    3    3  ]
//                            [                    ]
//                            [  1         1    1  ]
//                            [  -   - 1   -    -  ]
//                            [  3         3    3  ]
//                            [                    ]
//                            [  1    1         1  ]
//                            [  -    -   - 1   -  ]
//                            [  3    3         3  ]
//                            [                    ]
//                            [  1    1    1       ]
//                            [  -    -    -   - 1 ]
//                            [  3    3    3       ]

// Exp(Q.t) = 
// 
//        [       4 t               4 t            4 t            4 t  ]
//        [     - ---             - ---          - ---          - ---  ]
//        [        3                 3              3              3   ]
//        [ 3 %E        1   1   %E         1   %E         1   %E       ]
//        [ --------- + -   - - -------    - - -------    - - -------  ]
//        [     4       4   4      4       4      4       4      4     ]
//        [                                                            ]
//        [          4 t         4 t               4 t            4 t  ]
//        [        - ---       - ---             - ---          - ---  ]
//        [           3           3                 3              3   ]
//        [  1   %E        3 %E        1   1   %E         1   %E       ]
//        [  - - -------   --------- + -   - - -------    - - -------  ]
//        [  4      4          4       4   4      4       4      4     ]
//        [                                                            ]
//        [          4 t            4 t         4 t               4 t  ]
//        [        - ---          - ---       - ---             - ---  ]
//        [           3              3           3                 3   ]
//        [  1   %E         1   %E        3 %E        1   1   %E       ]
//        [  - - -------    - - -------   --------- + -   - - -------  ]
//        [  4      4       4      4          4       4   4      4     ]
//        [                                                            ]
//        [          4 t            4 t            4 t         4 t     ]
//        [        - ---          - ---          - ---       - ---     ]
//        [           3              3              3           3      ]
//        [  1   %E         1   %E         1   %E        3 %E        1 ]
//        [  - - -------    - - -------    - - -------   --------- + - ]
//        [  4      4       4      4       4      4          4       4 ]

double JCnuc::Pij_t(int i, int j, double d) const {
	if(i == j) return 1./4. + 3./4. * exp(- 4./3. * d);
	else       return 1./4. - 1./4. * exp(- 4./3. * d);
}

/******************************************************************************/

// d(Exp(Q,t))/dt = 
//                [                 4 t        4 t        4 t  ]
//                [       4 t     - ---      - ---      - ---  ]
//                [     - ---        3          3          3   ]
//                [        3    %E         %E         %E       ]
//                [ - %E        -------    -------    -------  ]
//                [                3          3          3     ]
//                [                                            ]
//                [      4 t                   4 t        4 t  ]
//                [    - ---         4 t     - ---      - ---  ]
//                [       3        - ---        3          3   ]
//                [  %E               3    %E         %E       ]
//                [  -------   - %E        -------    -------  ]
//                [     3                     3          3     ]
//                [                                            ]
//                [      4 t        4 t                   4 t  ]
//                [    - ---      - ---         4 t     - ---  ]
//                [       3          3        - ---        3   ]
//                [  %E         %E               3    %E       ]
//                [  -------    -------   - %E        -------  ]
//                [     3          3                     3     ]
//                [                                            ]
//                [      4 t        4 t        4 t             ]
//                [    - ---      - ---      - ---         4 t ]
//                [       3          3          3        - --- ]
//                [  %E         %E         %E               3  ]
//                [  -------    -------    -------   - %E      ]
//                [     3          3          3                ]

double JCnuc::dPij_dt(int i, int j, double d) const {
	if(i == j) return -       exp(- 4./3. * d);
	else       return 1./3. * exp(- 4./3. * d);
}

/******************************************************************************/

// d2(Exp(Q,t))/dt2 = 
//            [        4 t           4 t          4 t          4 t ]
//            [      - ---         - ---        - ---        - --- ]
//            [         3             3            3            3  ]
//            [  4 %E          4 %E         4 %E         4 %E      ]
//            [  ---------   - ---------  - ---------  - --------- ]
//            [      3             9            9            9     ]
//            [                                                    ]
//            [         4 t         4 t           4 t          4 t ]
//            [       - ---       - ---         - ---        - --- ]
//            [          3           3             3            3  ]
//            [   4 %E        4 %E          4 %E         4 %E      ]
//            [ - ---------   ---------   - ---------  - --------- ]
//            [       9           3             9            9     ]
//            [                                                    ]
//            [         4 t          4 t         4 t           4 t ]
//            [       - ---        - ---       - ---         - --- ]
//            [          3            3           3             3  ]
//            [   4 %E         4 %E        4 %E          4 %E      ]
//            [ - ---------  - ---------   ---------   - --------- ]
//            [       9            9           3             9     ]
//            [                                                    ]
//            [         4 t          4 t          4 t         4 t  ]
//            [       - ---        - ---        - ---       - ---  ]
//            [          3            3            3           3   ]
//            [   4 %E         4 %E         4 %E        4 %E       ]
//            [ - ---------  - ---------  - ---------   ---------  ]
//            [       9            9            9           3      ]

double JCnuc::d2Pij_dt2(int i, int j, double d) const {
	if(i == j) return   4./3. * exp(- 4./3. * d);
	else       return - 4./9. * exp(- 4./3. * d);
}

/******************************************************************************/

Mat JCnuc::getPij_t(double d) const {
	Mat p(_size, _size);
	for(unsigned int i = 0; i < _size; i++) {
		for(unsigned int j = 0; j < _size; j++) {
			p(i,j) = (i==j) ? 1./4. + 3./4. * exp(- 4./3. * d) : 1./4. - 1./4. * exp(- 4./3. * d);
		}
	}
	return p;
}

Mat JCnuc::getdPij_dt(double d) const {
	Mat p(_size, _size);
	for(unsigned int i = 0; i < _size; i++) {
		for(unsigned int j = 0; j < _size; j++) {
			p(i,j) = (i==j) ? - exp(- 4./3. * d) : 1./3. * exp(- 4./3. * d);
		}
	}
	return p;
}

Mat JCnuc::getd2Pij_dt2(double d) const {
	Mat p(_size, _size);
	for(unsigned int i = 0; i < _size; i++) {
		for(unsigned int j = 0; j < _size; j++) {
			p(i,j) = (i==j) ? 4./3. * exp(- 4./3. * d) : - 4./9. * exp(- 4./3. * d);
		}
	}
	return p;
}

/******************************************************************************/

string JCnuc::getName() const {
	return string("Jukes and Cantor (1969) for nucleotides");
}

/******************************************************************************/