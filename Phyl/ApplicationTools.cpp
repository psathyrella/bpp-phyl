//
// File: ApplicationTools.h
// Created by: Julien Dutheil <Julien.Dutheil@univ-montp2.fr>
// Created on: Sun Dec 14 09:36:26 2003
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

#include "ApplicationTools.h"
#include "JCnuc.h"
#include "K80.h"
#include "T92.h"
#include "HKY85.h"
#include "TN93.h"
#include "DSO78.h"
#include "JTT92.h"
#include "JCprot.h"
#include "OptimizationTools.h"
#include "Tree.h"
#include "Newick.h"
#include "HomogeneousTreeLikelihood.h"

// From Utils:
#include <Utils/FileTools.h>
#include <Utils/TextTools.h>
#include <Utils/StringTokenizer.h>

// From SeqLib:
#include <Seq/DNA.h>
#include <Seq/RNA.h>
#include <Seq/NucleicAlphabet.h>
#include <Seq/ProteicAlphabet.h>
#include <Seq/ISequence.h>
#include <Seq/Fasta.h>
#include <Seq/Mase.h>
#include <Seq/MaseTools.h>
#include <Seq/Phylip.h>
#include <Seq/Clustal.h>
#include <Seq/SiteContainerTools.h>

// From NumCalc:
#include <NumCalc/ConstantDistribution.h>
#include <NumCalc/GammaDiscreteDistribution.h>

// From the STL:
#include <fstream>
#include <iomanip>
using namespace std;

/******************************************************************************/

ostream & ApplicationTools::error   = cerr;
ostream & ApplicationTools::message = cout;
ostream & ApplicationTools::warning = cout;

/******************************************************************************/

bool ApplicationTools::parameterExists(
	const string & parameterName,
	map<string, string> & params)
{
	return params.find(parameterName) != params.end();
}
	
/******************************************************************************/

string ApplicationTools::getAFilePath(
	const string & parameter,
	map<string, string> & params,
	bool isRequired,
	bool mustExist,
	const string & suffix,
	bool suffixIsOptional)
{
	string filePath = getStringParameter(parameter, params, "none", suffix, suffixIsOptional, false);
	if(filePath == "") filePath = "none";
	if(filePath == "none" && isRequired) {
		displayError("You must specify a file for this parameter: " + parameter);
		exit(-1);
	}
	if(filePath == "none") return filePath;
   	if(mustExist && !FileTools::fileExists(filePath)) {
  		displayError("File does not exists: " + filePath);
		exit(-1);
	}
	return filePath;
}

/******************************************************************************/

double ApplicationTools::getDoubleParameter(
	const string & parameterName,
	map<string, string> & params,
	double defaultValue,
	const string & suffix,
	bool suffixIsOptional,
	bool warn)
{
	double dParam = defaultValue;
	if(parameterExists(parameterName + suffix, params)) {
		dParam = TextTools::toDouble(params[parameterName + suffix]);
	} else if(suffixIsOptional && parameterExists(parameterName, params)) {
		dParam = TextTools::toDouble(params[parameterName]);
	} else if(warn) {
		displayWarning("Parameter " + parameterName + suffix + " not specified. Default used instead: " + TextTools::toString(defaultValue));
	}
	return dParam;
}

/******************************************************************************/

int ApplicationTools::getIntParameter(
	const string & parameterName,
	map<string, string> & params,
	int defaultValue,
	const string & suffix,
	bool suffixIsOptional,
	bool warn)
{
	int iParam = defaultValue;
	if(parameterExists(parameterName + suffix, params)) {
		iParam = TextTools::toInt(params[parameterName + suffix]);
	} else if(suffixIsOptional && parameterExists(parameterName, params)) {
		iParam = TextTools::toInt(params[parameterName]);
	} else if(warn) {
		displayWarning("Parameter " + parameterName + suffix + " not specified. Default used instead: " + TextTools::toString(defaultValue));
	}
	return iParam;
}

/******************************************************************************/

string ApplicationTools::getStringParameter(
	const string & parameterName,
	map<string, string> & params,
	string defaultValue,
	const string & suffix,
	bool suffixIsOptional,
	bool warn)
{
	string sParam = defaultValue;
	if(parameterExists(parameterName + suffix, params)) {
		sParam = params[parameterName + suffix];
	} else if(suffixIsOptional && parameterExists(parameterName, params)) {
		sParam = params[parameterName];
	} else if(warn) {
		displayWarning("Parameter " + parameterName + " not specified. Default used instead: " + defaultValue);
	}
	return sParam;
}

/******************************************************************************/

bool ApplicationTools::getBooleanParameter(
	const string & parameterName,
	map<string, string> & params,
	bool defaultValue,
	const string & suffix,
	bool suffixIsOptional,
	bool warn)
{
	bool bParam = defaultValue;
	if(parameterExists(parameterName + suffix, params)) {
		string sParam = params[parameterName + suffix];
		bParam = (sParam == "true") 
		      || (sParam == "TRUE")
	          || (sParam == "t")
		      || (sParam == "T")
			  || (sParam == "yes")
			  || (sParam == "YES")
			  || (sParam == "y")
			  || (sParam == "Y")
		      || (sParam == "1");
	} else if(suffixIsOptional && parameterExists(parameterName, params)) {
		string sParam = params[parameterName];
		bParam = (sParam == "true") 
		      || (sParam == "TRUE")
	          || (sParam == "t")
		      || (sParam == "T")
			  || (sParam == "yes")
			  || (sParam == "YES")
			  || (sParam == "y")
			  || (sParam == "Y")
		      || (sParam == "1");
	} else if(warn) {
		displayWarning("Parameter " + parameterName + " not specified. Default used instead: " + TextTools::toString(defaultValue));
	}
	return bParam;
}

/******************************************************************************/

void ApplicationTools::displayMessage(const string & text) { message << text << endl; }
		
void ApplicationTools::displayError(const string & text) { error << "ERROR!!! " << text << endl; }
		
void ApplicationTools::displayWarning(const string & text) { warning << "WARNING!!! " << text << endl; }

void ApplicationTools::displayTask(const string & text) {
	message << TextTools::resizeRight(text, 40, '.') << ": ";
	message.flush();
}
		
void ApplicationTools::displayTaskDone() { message << "Done." << endl; }

void ApplicationTools::displayResult(const string & text, const string & result) {
	displayMessage(TextTools::resizeRight(text, 40, '.') + ": " + result);
}

/******************************************************************************/

Alphabet * ApplicationTools::getAlphabet(
	map<string, string> & params,
	const string & suffix,
	bool suffixIsOptional,
	bool verbose)
{
	Alphabet * chars;
	string alphabet = getStringParameter("alphabet", params, "DNA", suffix, suffixIsOptional);
	if(alphabet == "DNA") {
		chars = new DNA();
  	} else if (alphabet == "RNA") {
		chars = new RNA();
  	} else if (alphabet == "Protein") {
		chars = new ProteicAlphabet();
	} else {
		error << "Alphabet not known: " << alphabet << endl;
		exit(-1);
	}
	if(verbose) displayResult("Alphabet type", alphabet);
	return chars;
}

/******************************************************************************/

Tree<Node> * ApplicationTools::getTree(
	map<string, string> & params,
	const string & suffix,
	bool suffixIsOptional,
	bool verbose)
{
	string treeFilePath = getAFilePath("tree.file", params, true, true, suffix, suffixIsOptional);
	
	//Read the tree file:
	Newick newick(true);
	Tree<Node> * tree = newick.read(treeFilePath);
	if(verbose) displayResult("Tree file", treeFilePath);
	return tree;
}

/******************************************************************************/

VectorSiteContainer * ApplicationTools::getSiteContainer(
	const Alphabet * alpha,
	map<string, string> & params,
	const string & suffix,
	bool suffixIsOptional,
	bool verbose
) {
	string sequenceFilePath = getAFilePath("sequence.file",params, true, true, suffix, suffixIsOptional);
	string sequenceFormat = getStringParameter("sequence.format", params, "Fasta", suffix, suffixIsOptional);
  	if(verbose) displayResult("Sequence format", sequenceFormat);
	ISequence * iSeq = NULL;
	if(sequenceFormat == "Mase") {
		iSeq = new Mase();
	} else if(sequenceFormat == "Phylip")  {
		bool sequential = true, extended = true;
		if(params.find("sequence.format_phylip.order") != params.end()) {
			     if(params["sequence.format_phylip.order"] == "sequential" ) sequential = true;
			else if(params["sequence.format_phylip.order"] == "interleaved") sequential = false;
			else displayWarning("Argument '" +
						 params["sequence.format_phylip.order"] +
						 "' for parameter 'sequence.format_phylip.order' is unknown. " +
						 "Default used instead: sequential.");
		} else displayWarning("Argument 'sequence.format_phylip.order' not found. Default used instead: sequential.");
		if(params.find("sequence.format_phylip.ext") != params.end()) {
			     if(params["sequence.format_phylip.ext"] == "extended") extended = true;
			else if(params["sequence.format_phylip.ext"] == "classic" ) extended = false;
			else displayWarning("Argument '" +
						 params["sequence.format_phylip.ext"] +
						 "' for parameter 'sequence.format_phylip.ext' is unknown. " +
						 "Default used instead: extended.");
		} else displayWarning("Argument 'sequence.format_phylip.ext' not found. Default used instead: extended.");
		iSeq = new Phylip(extended, sequential);
	} else if(sequenceFormat == "Fasta") iSeq = new Fasta();
	else if(sequenceFormat == "Clustal") iSeq = new Clustal();
	else {
		displayError("Unknown sequence format.");
		exit(-1);
	}
	const SequenceContainer * seqCont = iSeq -> read(sequenceFilePath, alpha);
	VectorSiteContainer * sites = new VectorSiteContainer(* dynamic_cast<const OrderedSequenceContainer *>(seqCont));
    delete seqCont;
	delete iSeq;
	
  	if(verbose) displayResult("Sequence file " + suffix, sequenceFilePath);

	// Look for site selection:
	if(sequenceFormat == "Mase") {
		//getting site set:
		string siteSet = getStringParameter("sequence.format_mase.site_selection", params, "none", suffix, suffixIsOptional, false);
		if(siteSet != "none") {
            VectorSiteContainer * selectedSites;
            try {
                selectedSites = dynamic_cast<VectorSiteContainer *>(MaseTools::getSelectedSites(* sites, siteSet));
                if(verbose) displayResult("Set found", TextTools::toString(siteSet) + " sites.");
            } catch(IOException ioe) {
				displayError("Site Set '" + siteSet + "' not found.");
				exit(-1);
            }
            if(selectedSites -> getNumberOfSites() == 0) {
				displayError("Site Set '" + siteSet + "' is empty.");
				exit(-1);
            }
            delete sites;
            sites = selectedSites;
            //Old stuff:
            //SiteSelection selection = MaseTools::getSet(sites -> getGeneralComments(), siteSet);
			//int s = selection.size();
			//if(s == 0) {
			//	displayError("Site Set '" + siteSet + "' void or not found.");
			//	exit(-1);
			//} else {
			//	if(verbose) displayResult("Set found", TextTools::toString(s) + " sites.");
			//	AlignedSequenceContainer * selectedSites = MaseTools::getSelectedSites(* sites, selection);
			//	delete sites;
			//sites = new VectorSiteContainer(* selectedSites);
			//delete selectedSites;
			//}
		}
	}
	return sites;
}

/******************************************************************************/

VectorSiteContainer * ApplicationTools::getSitesToAnalyse(
	const SiteContainer & allSites,
	map<string, string> & params,
	string suffix,
	bool suffixIsOptional,
	bool verbose)
{
	// Fully resolved sites, i.e. without jokers and gaps:
	VectorSiteContainer * sitesToAnalyse;
	
	string option = getStringParameter("sequence.sites_to_use", params, "complete", suffix, suffixIsOptional);
	if(verbose) displayResult("Sites to use", option);

    if(option == "complete") {
		sitesToAnalyse = dynamic_cast<VectorSiteContainer *>(SiteContainerTools::getCompleteSites(allSites));
		int nbSites = sitesToAnalyse -> getNumberOfSites();
		if(verbose) displayResult("Complete sites", TextTools::toString(nbSites));
	} else if(option == "nogap") {
		sitesToAnalyse = dynamic_cast<VectorSiteContainer *>(SiteContainerTools::getSitesWithoutGaps(allSites));
		int nbSites = sitesToAnalyse -> getNumberOfSites();
		if(verbose) displayResult("Sites without gap", TextTools::toString(nbSites));
	} else {
		displayError("Option '" + option + "' unknown in parameter 'sequence.sitestouse'.");
		exit(-1);
	}

	return sitesToAnalyse;
}

/******************************************************************************/

SubstitutionModel * ApplicationTools::getSubstitutionModel(
	const Alphabet * alphabet,
	const SiteContainer * data,
	map<string, string> & params,
	const string & suffix,
	bool suffixIsOptional,
	bool verbose)
{
	string modelName = getStringParameter("model", params, "JCnuc", suffix, suffixIsOptional);
	SubstitutionModel * model = NULL;

	if(alphabet -> getAlphabetType() == "DNA alphabet"
	|| alphabet -> getAlphabetType() == "RNA alphabet") {

		const NucleicAlphabet * alpha = dynamic_cast<const NucleicAlphabet *>(alphabet);

		if(modelName == "TN93") {
			double piA = 0.25, piC = 0.25, piG = 0.25, piT = 0.25;
				double kappa1 = getDoubleParameter("kappa1", params, 2, suffix, suffixIsOptional);
				double kappa2 = getDoubleParameter("kappa2", params, 2, suffix, suffixIsOptional);
			bool useObsFreq = getBooleanParameter("model.use_observed_freq", params, false, suffix, suffixIsOptional);
			if(useObsFreq && data != NULL) {
				model = new TN93(alpha, kappa1, kappa2);
				dynamic_cast<TN93 *>(model) -> setFreqFromData(*data);
				piA = model -> getParameterValue("piA");
				piC = model -> getParameterValue("piC");
				piG = model -> getParameterValue("piG");
				piT = model -> getParameterValue("piT");
			} else {
				piA = getDoubleParameter("piA", params, 0.25, suffix, suffixIsOptional);
				piC = getDoubleParameter("piC", params, 0.25, suffix, suffixIsOptional);
				piG = getDoubleParameter("piG", params, 0.25, suffix, suffixIsOptional);
				piT = getDoubleParameter("piT", params, 0.25, suffix, suffixIsOptional);
				if( fabs(1-(piA + piT + piG + piC)) > 0.00000000000001 ) {
					displayError("Equilibrium base frequencies must equal 1. Aborting...");
					exit(-1);
				}
				model = new TN93(alpha, kappa1, kappa2, piA, piC, piG, piT);
			}
			if(verbose) {
				displayResult("model" , modelName);
				displayResult("kappa1", TextTools::toString(kappa1));
				displayResult("kappa2", TextTools::toString(kappa2));
				displayResult("piA"   , TextTools::toString(piA));
				displayResult("piC"   , TextTools::toString(piC));
				displayResult("piG"   , TextTools::toString(piG));
				displayResult("piT"   , TextTools::toString(piT));
			}
		} else if(modelName == "HKY85") {
			double piA = 0.25, piC = 0.25, piG = 0.25, piT = 0.25;
			double kappa = getDoubleParameter("kappa", params, 2, suffix, suffixIsOptional);
			bool useObsFreq = getBooleanParameter("model.use_observed_freq", params, false, suffix, suffixIsOptional);
			if(useObsFreq && data != NULL) {
				model = new HKY85(alpha, kappa);
				dynamic_cast<HKY85 *>(model) -> setFreqFromData(*data);
				piA = model -> getParameterValue("piA");
				piC = model -> getParameterValue("piC");
				piG = model -> getParameterValue("piG");
				piT = model -> getParameterValue("piT");
			} else {
				piA = getDoubleParameter("piA", params, 0.25, suffix, suffixIsOptional);
				piC = getDoubleParameter("piC", params, 0.25, suffix, suffixIsOptional);
				piG = getDoubleParameter("piG", params, 0.25, suffix, suffixIsOptional);
				piT = getDoubleParameter("piT", params, 0.25, suffix, suffixIsOptional);
				if( fabs(1-(piA + piT + piG + piC)) > 0.00000000000001 ) {
					displayError("Equilibrium base frequencies must equal 1. Aborting...");
					exit(-1);
				}
				model = new HKY85(alpha, kappa, piA, piC, piG, piT);
			}
			if(verbose) {
				displayResult("model", modelName);
				displayResult("kappa", TextTools::toString(kappa));
				displayResult("piA"  , TextTools::toString(piA));
				displayResult("piC"  , TextTools::toString(piC));
				displayResult("piG"  , TextTools::toString(piG));
				displayResult("piT"  , TextTools::toString(piT));
			}
		} else if(modelName == "T92") {
			double kappa = getDoubleParameter("kappa", params, 2, suffix, suffixIsOptional);
			double theta = 0.5;
			bool useObsFreq = getBooleanParameter("model.use_observed_freq", params, false, suffix, suffixIsOptional);
			if(useObsFreq && data != NULL) {
				model = new T92(alpha, kappa);
				dynamic_cast<T92 *>(model) -> setFreqFromData(*data);
				theta = model -> getParameterValue("theta");
			} else {
				theta = getDoubleParameter("theta", params, 0.5, suffix, suffixIsOptional);
				model = new T92(alpha, kappa, theta);
			}
			if(verbose) {
				displayResult("model", modelName);
				displayResult("kappa", TextTools::toString(kappa));
				displayResult("theta", TextTools::toString(theta));
			}
		} else if(modelName == "K80") {
    	double kappa = getDoubleParameter("kappa", params, 2, suffix, suffixIsOptional);
			model = new K80(alpha, kappa);
  		if(verbose) {
				displayResult("model", modelName);
				displayResult("kappa", TextTools::toString(kappa));
			}
  	} else if(modelName == "JCnuc") {
			model = new JCnuc(alpha);
  		if(verbose) {
				displayResult("model", modelName);
			}
		} else {
			displayError("Model '" + modelName + "' unknown. Aborting..."); //It would be better to throw an exception!
			exit(-1);
		}
	} else { // Alphabet supposed to be proteic!
		const ProteicAlphabet * alpha = dynamic_cast<const ProteicAlphabet *>(alphabet);
		bool useObsFreq = getBooleanParameter("model.use_observed_freq", params, false, suffix, suffixIsOptional);
		if(modelName == "JCprot") {
			model = new JCprot(alpha);
		}	else if(modelName == "DSO78") {
			model = new DSO78(alpha);
			if(useObsFreq && data != NULL) dynamic_cast<DSO78 *>(model) -> setFreqFromData(*data);
		} else if(modelName == "JTT92") {
			model = new JTT92(alpha);
			if(useObsFreq && data != NULL) dynamic_cast<JTT92 *>(model) -> setFreqFromData(*data);	
		} else {
			displayError("Model '" + modelName + "' unknown. Aborting...");
		}
 		if(verbose) {
			displayResult("model", modelName + (useObsFreq && (model != NULL) ? "-F" : ""));
		}
	}
	return model;
}

/******************************************************************************/

void ApplicationTools::printSubstitutionModelHelp() {
	message << "Substitution Model:" << endl;
	message << "model               | [JCnuc, K80, T92, HKY85, TN93] Model to use (default to JC)" << endl;
  	message << "kappa               | kappa parameter in Q matrix" << endl;
  	message << "kappa1              | kappa1 parameter in Q matrix" << endl;
  	message << "kappa2              | kappa2 parameter in Q matrix" << endl;
  	message << "theta               | theta parameter in Q matrix" << endl;
  	message << "piA                 | piA   parameter in Q matrix" << endl;
  	message << "pi²T                | piT   parameter in Q matrix" << endl;
  	message << "piC                 | piC   parameter in Q matrix" << endl;
  	message << "piG                 | piG   parameter in Q matrix" << endl;
	message << "use_observed_freq   | Tell if the observed frequencies must be used." << endl; 
}

/******************************************************************************/

DiscreteDistribution * ApplicationTools::getRateDistribution(
	map<string, string> & params,
	string suffix,
	bool suffixIsOptional,
	bool verbose
)
{
	string distributionType = getStringParameter("rate_distribution", params, "constant", suffix, suffixIsOptional);
	DiscreteDistribution * rDist;
	if(distributionType == "constant") {
		rDist = new ConstantDistribution(1.);
		if(verbose) {
			displayResult("rate_distribution", distributionType);
		}
	} else if(distributionType == "gamma") {
		double alpha = getDoubleParameter("rate_distribution_gamma.alpha", params, 1., suffix, suffixIsOptional);
		int nbClasses = getIntParameter("rate_distribution.classes_number", params, 4, suffix, suffixIsOptional);

		if(alpha < 0) {
			displayError("Alpha parameter in gamma distribution of rates must be > 0, found " + TextTools::toString(alpha) + ".");
			exit(-1);
		} else {
			rDist = new GammaDiscreteDistribution(nbClasses, alpha);
			if(verbose) {
				displayResult("Rate distribution", distributionType);
				displayResult("shape", TextTools::toString((dynamic_cast<GammaDiscreteDistribution *>(rDist)) -> getParameterValue("alpha")));
				displayResult("# classes", TextTools::toString(rDist -> getNumberOfCategories()));
				for(unsigned int c = 0; c < rDist -> getNumberOfCategories(); c++) {
					displayResult("* Category " + TextTools::toString(c)
					+ "(rate = " + TextTools::toString(rDist -> getCategory(c))
					+ "), prob = ", TextTools::toString(rDist -> getProbability(c)));
				}
			}
		}
	} else {
		displayError("Distribution unknown: " + distributionType + ".");
		exit(-1);
	}

	return rDist;
}

/******************************************************************************/

void ApplicationTools::printRateDistributionHelp() {
  	message << "rate_distribution   | uniform or gamma." << endl;
	message << "shape               | the gamma law's alpha parameter or -1 for estimate (default to -1)." << endl;
	message << "classes_number      | discrete approximation: number of categories (default to 4)." << endl;
}

/******************************************************************************/

void ApplicationTools::optimizeParameters(
	TreeLikelihood * tl,
	map<string, string> & params,
	const string & suffix,
	bool suffixIsOptional,
	bool verbose)
	throw (Exception)
{
	bool optimize = getBooleanParameter("optimization", params, true, suffix, suffixIsOptional, false);
	if(!optimize) return;
		
	string optMet = getStringParameter("optimization.method", params, "downhill+simplex", suffix, suffixIsOptional);
	if(verbose) displayResult("Optimization method", optMet);

	unsigned int optVerbose = getParameter<unsigned int>("optimization.verbose", params, 2, suffix, suffixIsOptional);
	
	string mhPath = getAFilePath("optimization.message_handler", params, false, false, suffix, suffixIsOptional);
	ostream * messageHandler = 
		(mhPath == "none") ? NULL :
			(mhPath == "std") ? &cout :
				new ofstream(mhPath.c_str(), ios::out);
	if(verbose) displayResult("Message handler", mhPath);

	string prPath = getAFilePath("optimization.profiler", params, false, false, suffix, suffixIsOptional);
	ostream * profiler = 
		(prPath == "none") ? NULL :
			(prPath == "std") ? &cout :
				new ofstream(prPath.c_str(), ios::out);
	if(profiler != NULL) (*profiler) << setprecision(20);
	if(verbose) displayResult("Profiler", prPath);

	bool scaleFirst = getBooleanParameter("optimization.scale_first", params, true, suffix, suffixIsOptional, true);
	if(scaleFirst) {
		// We scale the tree before optimizing each branch length separately:
		if(verbose) displayMessage("Scaling the tree before optimizing each branch length separately.");
		double tolerance = getDoubleParameter("scale_opt.tolerance", params, .0001, suffix, suffixIsOptional, true);
		if(verbose) displayResult("Scaling tolerance", TextTools::toString(tolerance));
		int nbEvalMax = getIntParameter("scale_opt.max_number_f_eval", params, 1000000, suffix, suffixIsOptional, true);
		if(verbose) displayResult("Scaling max # f eval", TextTools::toString(nbEvalMax));
		int n = OptimizationTools::optimizeTreeScale(
			tl,
			tolerance,
			nbEvalMax,
			messageHandler,
			profiler);
		if(verbose) message << "Performed " << n << " function evaluations." << endl;
	}
	
	// Should I ignore some parameters?
	string paramListDesc = getStringParameter("optimization.ignore_parameter", params, "", suffix, suffixIsOptional, false);
	StringTokenizer st(paramListDesc, ",");
	while(st.hasMoreToken()) {
		try {
			dynamic_cast<AbstractHomogeneousTreeLikelihood *>(tl) -> ignoreParameter(st.nextToken());
		} catch(ParameterNotFoundException pnfe) {
			displayError("Parameter '" + pnfe.getParameter() + "' not found, and so can't be ignored!");
		} catch(exception e) {
			error << "DEBUB: ERROR!!! This functionality can only be used with HomogeneousTreeLikelihood for now." << endl;
		}
	}
	
	int nbEvalMax =  getIntParameter("optimization.max_number_f_eval", params, 1000000, suffix, suffixIsOptional);
	if(verbose) displayResult("Max # ML evaluations", TextTools::toString(nbEvalMax));
	
	double tolerance = getDoubleParameter("optimization.tolerance", params, .000001, suffix, suffixIsOptional);
	if(verbose) displayResult("Tolerance", TextTools::toString(tolerance));
	
	int n = 0;	
	if(optMet == "simplex+powell") {
		double sTol = getDoubleParameter("sp_tol", params, 0., suffix, suffixIsOptional);
		if(verbose) displayResult("Simplex tolerance", TextTools::toString(sTol));

		n = OptimizationTools::optimizeWithDownhillSimplexAndPowellMethod(
			tl,
			sTol,
			tolerance,
			nbEvalMax,
			messageHandler,
			profiler,
			optVerbose);
	} else if(optMet == "powell") {
		n = OptimizationTools::optimizeWithPowellMethod(
			tl,
			tolerance,
			nbEvalMax,
			messageHandler,
			profiler,
			optVerbose);
	} else if(optMet == "simplex") {
		n = OptimizationTools::optimizeWithDownhillSimplexMethod(
			tl,
			tolerance,
			nbEvalMax,
			messageHandler,
			profiler,
			optVerbose);
	} else if(optMet == "simplex+brent") {
		if(tl -> getParameters().getParameter("alpha") == NULL) {
			displayWarning("Simplex+Brent method can only be used with a gamma-distributed rate across sites.");
			displayWarning("Switch to Simplex method instead.");
			optMet == "simplex";
			n = OptimizationTools::optimizeWithDownhillSimplexMethod(
				tl,
				tolerance,
				nbEvalMax,
				messageHandler,
				profiler,
				optVerbose);
		} else {
			string prAlphaPath = getAFilePath("alpha_profiler", params, false, false, suffix, suffixIsOptional);
			ostream * profilerAlpha = 
				(prAlphaPath == "none") ? NULL :
					(prAlphaPath == "std") ? &cout :
						new ofstream(prAlphaPath.c_str(), ios::out);
			if(profilerAlpha != NULL) (*profilerAlpha) << setprecision(20);
			if(verbose) displayResult("Alpha profiler", prAlphaPath);
			n = OptimizationTools::optimizeWithDownhillSimplexMethodAlphaSeparately(
				tl,
				tolerance,
				nbEvalMax,
				messageHandler,
				profiler,
				profilerAlpha,
				optVerbose);
		}
	} else if(optMet == "powell+brent") {
		if(tl -> getParameters().getParameter("alpha") == NULL) {
			displayWarning("Powell+Brent method can only be used with a gamma-distributed rate across sites.");
			displayWarning("Switch to Powell method instead.");
			optMet == "powell";
			n = OptimizationTools::optimizeWithPowellMethod(
				tl,
				tolerance,
				nbEvalMax,
				messageHandler,
				profiler,
				optVerbose);
		} else {
			string prAlphaPath = getAFilePath("alpha_profiler", params, false, false, suffix, suffixIsOptional);
			ostream * profilerAlpha = 
				(prAlphaPath == "none") ? NULL :
					(prAlphaPath == "std") ? &cout :
						new ofstream(prAlphaPath.c_str(), ios::out);
			if(profilerAlpha != NULL) (*profilerAlpha) << setprecision(20);
			if(verbose) displayResult("Alpha profiler", prAlphaPath);
			n = OptimizationTools::optimizeWithPowellMethodAlphaSeparately(
				tl,
				tolerance,
				nbEvalMax,
				messageHandler,
				profiler,
				profilerAlpha,
				optVerbose);
		}
	} else if(optMet == "newton") {
		n = OptimizationTools::optimizeWithNewtonMethod(
			tl,
			tolerance,
			nbEvalMax,
			messageHandler,
			profiler,
			optVerbose);		
	} else if(optMet == "metanewton") {
		n = OptimizationTools::optimizeWithNewtonBrentMethod(
			dynamic_cast<AbstractHomogeneousTreeLikelihood *>(tl),
			tolerance,
			nbEvalMax,
			messageHandler,
			profiler,
			optVerbose);		
	} else {
		displayError("Method '" + optMet + "' unknown.");
		exit(-1);
	}
	if(verbose) displayResult("Performed", TextTools::toString(n) + " function evaluations.");
}

/******************************************************************************/

void ApplicationTools::printOptimizationHelp() {
	message << "Parameter optimization: keep or redo" << endl;
	message << "____________________________________________________________________" << endl;
	message << "branches_lengths| branches lengths." << endl;
	message << "subs_params     | parameters of the substitution model." << endl;
	message << "rate_dist       | rate distribution." << endl;
}

/******************************************************************************/

void ApplicationTools::writeTree(
	const Tree<Node> & tree,
	map<string, string> & params,
	const string & suffix,
	bool verbose)
{
	string file = getAFilePath("output.tree", params, true, false, suffix, false);
	Newick newick;
	newick.write(tree, file, true);
	if(verbose) displayMessage("Wrote tree to file '" + file + "'.");
}

/******************************************************************************/

void ApplicationTools::writeSequenceFile(
	const SequenceContainer & sequences,
	map<string, string> & params,
	const string & suffix,
	bool verbose)
{
	string file   = getAFilePath("output.sequence.file", params, true, false, suffix, false);
	string format = getStringParameter("output.sequence.format", params, "Fasta", suffix, false, true);
	int ncol      = getIntParameter("output.sequence.length", params, 100, suffix, false, false);
	OSequence * oSeq;
	if(format == "Fasta") oSeq = new Fasta(ncol);
	else if(format == "Mase") oSeq = new Mase(ncol);
	else if(format == "Phylip") {
		bool sequential = true, extended = true;
		if(params.find("output.sequence.format_phylip.order" + suffix) != params.end()) {
			     if(params["output.sequence.format_phylip.order"] == "sequential" ) sequential = true;
			else if(params["output.sequence.format_phylip.order"] == "interleaved") sequential = false;
			else displayWarning("Argument '" +
						 params["output.sequence.format_phylip.order"] +
						 "' for parameter 'output.sequence.format_phylip.order' is unknown. " +
						 "Default used instead: sequential.");
		} else displayWarning("Argument 'output.sequence.format_phylip.order' not found. Default used instead: sequential.");
		if(params.find("output.sequence.format_phylip.ext" + suffix) != params.end()) {
			     if(params["output.sequence.format_phylip.ext"] == "extended") extended = true;
			else if(params["output.sequence.format_phylip.ext"] == "classic" ) extended = false;
			else displayWarning("Argument '" +
						 params["output.sequence.format_phylip.ext"] +
						 "' for parameter 'output.sequence_phylip.ext' is unknown. " +
						 "Default used instead: extended.");
		} else displayWarning("Argument 'output.sequence_phylip.ext' not found. Default used instead: extended.");
		oSeq = new Phylip(extended, sequential, ncol);
	} else {
		displayError("Format '" + format + "' unknown.");
		exit(-1);
	}
	
	// Write sequences:
	oSeq -> write(file, sequences, true);
	
	delete oSeq;
}