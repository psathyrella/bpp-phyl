//
// File: Newick.h
// Created by: jdutheil <Julien.Dutheil@univ-montp2.fr>
// Created on: Thu Oct 23 15:35:03 2003
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

#include "Newick.h"
#include "TreeTools.h"

// From Utils:
#include <Utils/TextTools.h>

// From the STL:
#include <iostream>
#include <fstream>

using namespace std;

/******************************************************************************/

Newick::Newick(bool allowComments): _allowComments(allowComments) {}

Newick::~Newick() {}
	
/******************************************************************************/

const string Newick::getFormatName() const { return "Newick"; }

/******************************************************************************/

const string Newick::getFormatDescription() const
{
	return string("New hampshire parenthesis format. ") +
		"See http://evolution.genetics.washington.edu/phylip/newicktree.html for more info.";
}

/******************************************************************************/

Tree<Node> * Newick::read(istream & in) const throw (Exception)
{
	// Checking the existence of specified file
	if (! in) { throw IOException ("Newick::read : failed to read from stream"); }
	
	//We concatenate all line in file till we reach the ending semi colon:
	string temp, description;// Initialization
	// Main loop : for all file lines
	while (! in.eof()) {
		getline(in, temp, '\n');  // Copy current line in temporary string
		int index = temp.find(";");
		if(index >= 0 && index < (int)temp.size()) {
			description += temp.substr(0, index + 1);
			break;
		} else description += temp;
	}
	if(_allowComments) description = TextTools::removeSubstrings(description, '[', ']');
	return TreeTools::parenthesisToTree(description);
}

/******************************************************************************/

void Newick::write(const Tree<Node> & tree, ostream & out) const throw (Exception)
{
	// Checking the existence of specified file, and possibility to open it in write mode
	if (! out) { throw IOException ("Newick::write : failed to write to stream"); }
	out << TreeTools::treeToParenthesis(tree);
}

/******************************************************************************/