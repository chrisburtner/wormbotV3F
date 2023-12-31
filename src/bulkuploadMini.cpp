#include <sstream>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <cgicc/CgiDefs.h>
#include <cgicc/Cgicc.h>
#include <cgicc/HTTPHTMLHeader.h>
#include <cgicc/HTMLClasses.h>


#define WELL_STATE_START 2
#define WELL_STATE_ACTIVE 1
#define WELL_STATE_STOP 0

#define TRANSMITTED_ON 1
#define GFP_ON 1
#define CHERRY_ON 1
#define TIMELAPSE_ON 1
#define DAILY_MONITOR_ON -2

#define GFP_OFF 0
#define CHERRY_OFF 0
#define TRANSMITTED_OFF 0
#define TIMELAPSE_OFF 0
#define DAILY_MONITOR_OFF -1

//field order for TEMPLATE CSV
#define PLATE 0
#define ROW 1
#define WELL 2
#define TIMELAPSE 3
#define DAILYMONITOR 4
#define ACTIVATEUV 5
#define ACTIVATEGFP 6
#define ACTIVATECHERRY 7
#define EMAIL 8
#define INVESTIGATOR 9
#define TITLE 10
#define DESCRIPTION 11
#define STARTN 12
#define AGE 13
#define STRAIN 14



using namespace std;
using namespace cgicc;


//globals
Cgicc cgi;
string datapath;
stringstream externaldump;		//full processed file to dump

struct XYpair{
	int x;
	int y;
};

struct wellSort{
	int rank;
	string welldataline;

	bool operator < (const wellSort str) const {
		return (rank < str.rank);
	}
};

void sortJobList(void);

XYpair getXYpair(string queryline) {
	string filename;
	filename = datapath + string("platecoordinates.dat");
	ifstream ifile(filename.c_str());
	string readline;
	XYpair found;
	found.x=0;
	found.y=0;
	string token;

	while (getline(ifile,readline)) {
		if (readline.find(queryline) != std::string::npos) {
			stringstream isis(readline);
			getline(isis, token, ','); //remove wellname
			getline(isis, token, ','); //get x
			found.x = atoi(token.c_str());
			getline(isis, token, ','); //get y
			found.y = atoi(token.c_str());

			return(found);
		}
	}
	return(found);
}

long getNewExpID(void) {
	int fetchID;
	long expID;

	string filename;
	filename = datapath + string("currexpid.dat");

	ifstream ifile(filename.c_str());
	ifile >> expID;
	expID++;
	ifile.close();

	ofstream ofile(filename.c_str());
	ofile << expID << endl;
	ofile.close();

	return (expID);
}


vector<int> readOpenPlates(void){
	string inputfilename("RRRjoblist.csv");
	inputfilename = datapath + inputfilename;
     ifstream ifile(inputfilename.c_str());
     vector<int> openplates;

     for (int i=1; i <13; i++){
    	 openplates.push_back(i);
 //   	 cout << "add" << i;

     }//end pushback all of plates


     string inputline;
     string header;
      cout << hr()<< b("CURRENTLY ACTIVE EXPERIMENTS:") << br() << h2("select a checkbox to stop an experiment")<< br()<< endl;
      cout << "<div style=\"height:200px;color:white;background-color:crimson;border:1px solid#ccc; overflow:auto;\">" << endl;

     string token;
     getline(ifile,header);
     vector <int> platesinuse;
     while (getline(ifile, inputline)){
    	 stringstream ss(inputline);
    	 int status=0;
    	 int plate=0;
    	 string email;
    	 string investigator;
    	 string title;
    	 string description;
    	 string directory;
    	 string wellname;
    	 string activeTimelapse;
     	 string activeDailyMonitor;
    	 long expID=0;

//            cout << "inputline:" << inputline << endl;
    	 	 	 	 getline(ss, token, ',');
    	       		 expID = atol(token.c_str());
     				 getline(ss, token, ',');
      				 status = atoi(token.c_str());
     				 getline(ss, token, ',');
     				 plate = atoi(token.c_str());

     				 getline(ss, wellname, ',');
     				 getline(ss, token, ',');
     				 //xval = atoi(token.c_str());
     				 getline(ss, token, ',');
     				 //yval = atoi(token.c_str());
     				 getline(ss, directory, ',');
     				getline(ss, activeTimelapse, ',');
      				 getline(ss, activeDailyMonitor, ',');
     				 getline(ss, email, ',');
     				 getline(ss, investigator, ',');
     				 getline(ss, title, ',');
     				 getline(ss, description, ',');

     //				 cout << "status" << status << endl;
     				 if (status){
     					 platesinuse.push_back(plate);
     					 cout << br() << endl;
     					 stringstream  expValue;
     					 expValue << expID;
     					 cout << "<input type=\"checkbox\" name=\"" << expValue.str().c_str()<<"\">"  <<endl;
     					 cout << b("expID:") << expID << b(" plate:") << plate << b(" well:")<<wellname <<b(" investigator:")<<investigator<< b(" title:")<<title<< " "<<description << endl;
     					 cout << "<a href=\"/cgi-bin/marker?loadedexpID=" << expID <<"\" > ANALYZE </a>";
     					 cout << b("VERIFY STOP") << "<input type=\"checkbox\" name=\"v" << expValue.str().c_str()<<"\">" << hr() <<endl;
     				 }//end if active plate

     }//end while lines in file
     cout << "</div>" << endl;
     vector<int> returnplates;
     //remove used plates
     for (vector<int>::iterator citer = openplates.begin(); citer != openplates.end(); citer++){
         int matched=0;
    	 for (vector<int>::iterator jiter = platesinuse.begin(); jiter != platesinuse.end(); jiter++){
     //  		 cout << "platesinuse" << (*jiter) << endl;
    		 if ((*citer)==(*jiter)){

       			matched=1;

       			}

         }//end for
    	 if (!matched) returnplates.push_back(*citer);

     }//end for each plate
     return (returnplates);

}//readOpenPlates

void processDeletes(void){
	 string inputfilename("RRRjoblist.csv");
	 inputfilename = datapath + inputfilename;
	 ifstream ifile(inputfilename.c_str());
	 string header;
	 string inputline;
	 string token;
	 bool experimentstodelete = false;

	 stringstream ss;

	 vector<long> totalexperiments;
	 vector<string> deletedexperiments;

	 getline(ifile,header);

	      while (getline(ifile, inputline)){
	     	 stringstream ss(inputline);
	     	 int status=0;
	     	 int plate=0;
	     	 string email;
	     	 string investigator;
	     	 string title;
	     	 string description;
	     	 string directory;
	     	 string wellname;
	     	 string activeTimelapse;
	     	 string activeDailyMonitor;
	     	 long expID=0;

	 //            cout << "inputline:" << inputline << endl;
	     	 	 	 	 getline(ss, token, ',');
	     	       		 expID = atol(token.c_str());
	      				 getline(ss, token, ',');
	       				 status = atoi(token.c_str());
	      				 getline(ss, token, ',');
	      				 plate = atoi(token.c_str());

	      				 getline(ss, wellname, ',');
	      				 getline(ss, token, ',');
	      				 //xval = atoi(token.c_str());
	      				 getline(ss, token, ',');
	      				 //yval = atoi(token.c_str());
	      				 getline(ss, directory, ',');
	      				 getline(ss, activeTimelapse, ',');
	      				 getline(ss, activeDailyMonitor, ',');
	      				 getline(ss, investigator, ',');
	      				 getline(ss, title, ',');
	      				 getline(ss, description, ',');

	      //				 cout << "status" << status << endl;
	      				 if (status){
	      					 totalexperiments.push_back(expID);


	      				 }//end if active plate add to experiments

	      }//end while lines in file
	      ifile.close();

	      vector<long> purgeIDs;


	      for (vector<long>::iterator citer = totalexperiments.begin(); citer != totalexperiments.end(); citer++){
	    	  int todelete=0;
	    	  stringstream sss;
	    	  sss << (*citer);
	    	  todelete = cgi.queryCheckbox(sss.str());
	    	     if( todelete ) {

	    	    	 stringstream ssss;
	    	    	 ssss  << "v" << (*citer);

	    	    	 if(cgi.queryCheckbox(ssss.str())) {

	    	    		 purgeIDs.push_back((*citer)); //if both boxes checked delete it
	    	    		 experimentstodelete=true;
	    	    	 }

	    	     }//if delete selected

	      }//end for totalexperiments
	      if (experimentstodelete){
	    	  stringstream mover;
	    	  mover << "mv " << datapath << "RRRjoblist.csv "<< datapath <<"RRRold.csv";
	    	  system(mover.str().c_str());
	    	  string oldfilename;
	    	  oldfilename = datapath + string("RRRold.csv");
	    	  ifstream oldfile(oldfilename.c_str());
	    	  ofstream outfile(inputfilename.c_str());
	    	  string readline;
              getline(oldfile,readline);
	    	   //move header;
               //indicate the update
	    	  outfile << "UPDATE," << readline << endl;


              deleted:
	    	  while(getline(oldfile,readline)){

	    		  stringstream myss(readline);
	    		  getline(myss, token, ',');
	    		  long thisexpID;
	    		  thisexpID = atol(token.c_str());

	    	  	      for (vector<long>::iterator citer = purgeIDs.begin(); citer != purgeIDs.end(); citer++){
                         // cout << "purgeID: " << (*citer) << "thisexpID:" << thisexpID << endl;
	    	  	    	  if (thisexpID == (*citer)){
                        	 // cout << "TRUE";
                        	  deletedexperiments.push_back(readline);
                        	  goto deleted;
                          }//end if found a match
	    	  	      }//end for each to delete
	    	  	      outfile << readline << endl;
	    	  }//end while loop

	    	  for (vector<string>::iterator citer = deletedexperiments.begin(); citer != deletedexperiments.end(); citer++){
	    		  string exp;
	    		  string remainder;
	    		  stringstream dss((*citer));
	    		  getline(dss, exp, ','); //save the expID
	    		  getline(dss,token,','); //discard the old status
	    		  getline(dss,remainder); //save the rest
	    		  outfile << exp << ","<< WELL_STATE_STOP << "," << remainder << endl;

	    	  }//end for each deleted experimetn

	    	  outfile.close();

	    	 // sortJobList();

	     }//end if need to delete




	   cout << "<br/>\n";
}//end processDeletes

void sortJobList(void){

	vector<string> wellorder;
	vector<wellSort> wellsToSort;


		string filename;
		filename = datapath + string("platecoordinates.dat");
		ifstream ifile(filename.c_str());
		string readline;

		string token;

		while (getline(ifile,readline)){
			stringstream awell(readline);
			string thewell;
			getline(awell,thewell,',');

			wellorder.push_back(thewell);

		}//end while lines in the file


		string inputfilename("RRRjoblist.csv");
	    inputfilename = datapath + inputfilename;
		ifstream wellfile(inputfilename.c_str());

		string header; //save the header
		getline(wellfile,header);
		string aline;
		while (getline(wellfile,aline)){
			stringstream linestream(aline);
			string plate;
			string well;
			string junk;
			getline(linestream,junk,','); //discard expID
			getline(linestream,junk,','); //discard state
			getline(linestream,plate,','); //fetch plate
			getline(linestream,well,','); //fetch well

			stringstream query;
			query  << plate  << well;


			wellSort thiswellsort;
			thiswellsort.welldataline = aline;

			for (int i =0; i < wellorder.size(); i++){
				if (wellorder[i].find(query.str()) != std::string::npos){
					thiswellsort.rank=i;
					break;
				}//end if found the well
			}//end for each line in wellorder
			wellsToSort.push_back(thiswellsort);


		}//end while lines in the joblist

		std::sort(wellsToSort.begin(),wellsToSort.end());

		string outputfile("RRRjoblist.csv");
		outputfile = datapath + outputfile;
		ofstream ofile(outputfile.c_str());

		ofile << header << endl;
		for (int i=0;  i < wellsToSort.size(); i++){
			ofile << wellsToSort[i].welldataline << endl;
		}//end for each well
		ofile.close();






}//end sortJobList

string removeNastiness(string jerky){

	 boost::replace_all(jerky,",","COMMA");
	// boost::replace_all(jerky,".","PERIOD");
	 boost::replace_all(jerky,"/","SLASH");
	 boost::replace_all(jerky,"\\","BACKSLASH");
	 boost::replace_all(jerky,";","SEMICOLON");
	 boost::replace_all(jerky,":","COLON");
	 boost::replace_all(jerky,"~","TILDE");
	 boost::replace_all(jerky,"&","AND");
	 boost::replace_all(jerky,"|","PIPE");
 return (jerky);
}//end removeNastiness

void processInput(void){
	// Print out the submitted element
	stringstream iss;
	stringstream dump;
	int plate;
	bool doUpdate=false;
	plate = atoi(cgi("plate").c_str());

	for (int i=0; i <12; i++){
		iss.str(""); //clear stream
		iss << "active" <<i;
		stringstream oss;
		string dailymonitorcheck;
		dailymonitorcheck=string("adm") + boost::lexical_cast<string>(i);

		if (cgi.queryCheckbox(iss.str()) || cgi.queryCheckbox(dailymonitorcheck)){///\ if timelapse or monitor is setup then activate a new experiment
			doUpdate=true;
			string namevalue;
			stringstream findcoor;
			//build new line
			long expID;
			expID = getNewExpID();
			oss << expID << ",";
			oss << WELL_STATE_START << ",";
			oss << plate << ",";
			findcoor << plate; //save the value to find the x,y coordinates
			namevalue = "";
			namevalue =string("row") + boost::lexical_cast<string>(i);
			oss << removeNastiness(cgi(namevalue));
			findcoor << removeNastiness(cgi(namevalue));
			namevalue = "";
			namevalue ="well" + boost::lexical_cast<string>(i);
			oss << removeNastiness(cgi(namevalue)) << ",";
			findcoor << removeNastiness(cgi(namevalue));
			XYpair thisxy;
			thisxy = getXYpair(findcoor.str());
			oss << thisxy.x << ",";
			oss << thisxy.y << ",";
			string makedir;
			//expID and datapath is not read from form it is safe for system
			makedir = string("mkdir ") + datapath  + boost::lexical_cast<string>(expID);
			system(makedir.c_str()); //make the directory  ...should probably add error checking here
			oss << datapath << expID <<"/,"; //print the directory
			namevalue="";
			namevalue ="active" + boost::lexical_cast<string>(i);
			if (cgi.queryCheckbox(namevalue)) oss << TIMELAPSE_ON << ","; else oss << TIMELAPSE_OFF << ",";
			namevalue="";
			namevalue ="adm" + boost::lexical_cast<string>(i);
			if (cgi.queryCheckbox(namevalue)) oss << DAILY_MONITOR_ON << ","; else oss << DAILY_MONITOR_OFF << ",";
			namevalue = "";
			namevalue ="email" + boost::lexical_cast<string>(i);
			oss << removeNastiness(cgi(namevalue)) << ",";
			namevalue = "";
			namevalue ="investigator" + boost::lexical_cast<string>(i);
			oss << removeNastiness(cgi(namevalue)) << ",";
			namevalue = "";
			namevalue ="title" + boost::lexical_cast<string>(i);
			oss << removeNastiness(cgi(namevalue)) << ",";
			namevalue = "";
			namevalue ="description" + boost::lexical_cast<string>(i);
			oss << removeNastiness(cgi(namevalue)) << ",";
			namevalue = "";
			namevalue ="startn" + boost::lexical_cast<string>(i);
			oss << removeNastiness(cgi(namevalue)) << ",";
			namevalue = "";
			namevalue ="startage" + boost::lexical_cast<string>(i);
			oss << removeNastiness(cgi(namevalue)) << ",";
			int startingage = atoi(string(cgi(namevalue)).c_str()); ///store the starting age to add it to the day counter
			namevalue = "";
			namevalue ="strain" + boost::lexical_cast<string>(i);
			oss << removeNastiness(cgi(namevalue)) << ",";
			oss << "0,";  //write out the curr frame ZERO

			// write out current time
			time_t t;
			time(&t);
			oss << t;

			dump << oss.str() << endl;
		}//end if box active

	}//end for each well in a plate

	if (doUpdate){
		 string inputfilename("RRRjoblist.csv");
		 inputfilename = datapath + inputfilename;
		 ofstream ofile(inputfilename.c_str(), std::ios::app); //append new data to the oldfile
		 string outline;
		 while (getline(dump,outline)){
			 ofile << outline << endl;
		 }//end while lines to dump

		 ofile.close();



		 ifstream ifile(inputfilename.c_str()); //append new data to the oldfile

		 string header;
		 stringstream os;

		 getline(ifile,header);
		 os<< "UPDATE," + header << endl;
		 while (getline(ifile,outline)){
			 os << outline << endl;
		 }//end while lines to dump

		 ifile.close();

		 ofstream wfile(inputfilename.c_str()); //overwrite oldfile
		 string sendline;
		 while (getline(os,sendline)){
			 wfile << sendline << endl;
		 }//end while lines to dump

		 wfile.close();
		// sortJobList();
	}//end if doUpdate







}//end process input

void processALine(string csvdataline){
	string token;
	stringstream ss(csvdataline); //inputline
	stringstream oss;		//outputline
	

	long expID;
	expID = getNewExpID(); //fetch an experiment ID
	oss << expID << ",";   //print the expID
	oss << WELL_STATE_START << ","; //set the state to start
		
	int fieldcount=0;
	int plate;
	string therow;
	while(getline(ss,token,',') ){
	
		
		switch (fieldcount){
			case PLATE:
				{
				plate = atoi(token.c_str());
				oss << plate << ",";
				}
			break;

			case ROW:
				{
				therow=token;
				oss << therow; //merge with well no ,
				}
			break;

			case WELL:
				{
				int wellnum=atoi(token.c_str());
				oss << wellnum << ",";
				//get coordinates
				stringstream findit;
				findit << plate << therow << wellnum;
				XYpair thisxy;
				thisxy = getXYpair(findit.str());
				oss << thisxy.x << ",";
				oss << thisxy.y << ",";
				string makedir;
				//expID and datapath is not read from form it is safe for system
				makedir = string("mkdir ") + datapath  + boost::lexical_cast<string>(expID);
				system(makedir.c_str()); //make the experiment directory  ...should probably add error checking here
				oss << datapath << expID <<"/,"; //print the directory
				}
			break;

			case TIMELAPSE:
				{
				if (token.find("1") != std::string::npos) oss << TIMELAPSE_ON << ","; else oss << TIMELAPSE_OFF << ",";
				}
			break;

			case DAILYMONITOR:
				{			
				if (token.find("1") != std::string::npos) oss << DAILY_MONITOR_ON << ","; else oss << DAILY_MONITOR_OFF << ",";
				}
			break;

			case ACTIVATEUV:
			case ACTIVATEGFP:
			case ACTIVATECHERRY:
				{
				long exposureTime=abs(atol(token.c_str()));
				oss << exposureTime << ",";
				}
			break;


			case STARTN:
			case AGE:
				{
				int n=atoi(token.c_str());
				oss << n << ",";
				}
			break;

			case EMAIL:
			case TITLE:
			case DESCRIPTION:
			case INVESTIGATOR:
			case STRAIN:
				{
				oss << token << ",";
				}
			break;



		}//end switch
				
			

		

		fieldcount++;
	}//while fields in the line

	oss << "0,";  //write out the curr frame ZERO

	// write out current time
	time_t t;
	time(&t);
	oss << t;

	externaldump << oss.str() << endl;
	


}//end processALine



void processCSVFile(string inputfilename, string templatefilename){
	string fileline;
	ifstream ifile(inputfilename.c_str());
	if (!ifile.is_open()) {cout << "<h1>READ FILE FAILED!!!</h1>\n"; return;} //throw error if file not opened

	//open template and get the first 2 lines to check the uploaded file
	ifstream tfile(templatefilename.c_str());
	string key0,key1;
	getline(tfile,key0);
	getline(tfile,key1);
	tfile.close();

	int lcount=0;	
	int expcounter =0;
	while (getline(ifile,fileline)){
		if (lcount>0){	//ingnore the template first line
			if (fileline.find("plate number") != std::string::npos) {
				if (key1 != fileline){
					cout << "Line " << lcount <<  " column headers invalid match to current template" << endl;
					return;
				}

			} // check template headers for proper column names
			else { //not a template header try to process it
				processALine(fileline);
				expcounter++;
			}  


		}//if not on the first line
		else {

			if (key0 != fileline) {
				cout << "Line 0 bad in template <P>" << endl;
				cout << "key0=" << key0 << "<P>\n";
				cout << "fileline=" << fileline << "<P>\n";
				cout << "number of columns is probably off, remove invalid columns <P>" << endl;  
				return;
			
			}//end if first line doesn't match current template
		}//end if first line
		lcount++;
	}//end while lines in the file	
	ifile.close();

	cout << "<h1>" << expcounter << "experiments added<br></h1>" << endl; 



	 string jobfilename("RRRjoblist.csv");
	 jobfilename = datapath + jobfilename;
	 ofstream ofile(jobfilename.c_str(), std::ios::app); //append new data to the oldfile
	 string outline;
	 while (getline(externaldump,outline)){
		 ofile << outline << endl;
	 }//end while lines to dump

	 ofile.close();



	 ifstream iofile(jobfilename.c_str()); //read in the file to append the new header

	 string header;
	 stringstream os;

	 getline(iofile,header);
	 os<< "UPDATE," + header << endl;
	 while (getline(iofile,outline)){
		 os << outline << endl;
	 }//end while lines to dump

	 iofile.close();

	 ofstream wfile(jobfilename.c_str()); //overwrite oldfile
	 string sendline;
	 while (getline(os,sendline)){
		 wfile << sendline << endl;
	 }//end while lines to dump

	 wfile.close();



}//end processCSVFile


int main(int argc, char **argv) {

	bool passedfilters= false;
	try {

		ifstream readpath("data_path");

		
		

		readpath >> datapath;

		string bulkfilename  = datapath + "uploadedfile.csv";
		string templatefilename = datapath + "wormbot_template.csv";
		string remotefilename;

		ofstream outputexperiment(bulkfilename.c_str());  //a file to dump the upload to
		 const_file_iterator file = cgi.getFile("experiments"); //read in the file
		remotefilename=  file->getFilename();
		 file->writeToStream(outputexperiment); //save it to the wormbot directory
		outputexperiment.close();

		


		// Send HTTP header
		cout << HTTPHTMLHeader() << endl;

		// Set up the HTML document
		cout << html() << head(title("WormBot Bulk Upload Complete")) << endl;
		cout << body()  << "filname uploaded was: " << remotefilename << endl;

		//cout << img().set("src","http://kaeberleinlab.org/images/kaeberlein-lab-logo-2.png") << endl;

		if (remotefilename.find(".csv") == std::string::npos) {
			cout << "no valid file extension found" << endl;

		} else {
			cout << "file correctly named .csv" << endl;
			passedfilters = true;
		}   

		
		
		cout << br() <<endl;
		cout << img().set("src", "/wormbot/img/Bender.png").set("width","300") << endl;
		cout << br() <<endl;
		cout << "<h3>Status</h3>";
		if (passedfilters)
		processCSVFile(bulkfilename, templatefilename);
		else {
			cout << "CSV file not valid or improperly formatted.  Make sure you are using a current template and properly formated file and saving as a text only CSV using openoffice or something else besides Excel" << endl;

		}//end if failed

		//processDeletes();
		//processInput();
		//sortJobList();

		



		
		// Close the HTML document
		cout << body() << html();

	} catch(exception& e) {
		// handle any errors - omitted for brevity
	}
}
