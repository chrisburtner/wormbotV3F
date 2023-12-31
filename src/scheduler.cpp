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

#define GFP_ON 1
#define CHERRY_ON 1
#define UV_ON 1
#define TRANSMITTED_ON 1
#define TIMELAPSE_ON 1
#define DAILY_MONITOR_ON -2


#define GFP_OFF 0
#define CHERRY_OFF 0
#define UV_OFF 0
#define TRANSMITTED_OFF 0
#define TIMELAPSE_OFF 0
#define DAILY_MONITOR_OFF -1


using namespace std;
using namespace cgicc;


//globals
Cgicc cgi;
string datapath;

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

void loadCss(void) {
	string component_path = datapath + "scheduler_parts/"; // TODO
	string filename = "scheduler.css";
	filename = component_path + filename;
	//cout << "this is the filename:" << filename << endl;
	
	ifstream cssStream;
	cssStream.open(filename);
	string css;
	cout << "<style>" << endl;
	while (getline(cssStream, css)) {
		css += "\n";
		cout << css << endl;
	}
	cout << "</style>" << endl;
}

vector<int> readOpenPlates(void){
	string inputfilename("RRRjoblist.csv");
	inputfilename = datapath + inputfilename;
     ifstream ifile(inputfilename.c_str());
     vector<int> openplates;

     for (int i=1; i <13; i++){ //change for mini
    	 openplates.push_back(i);
 //   	 cout << "add" << i;

     }//end pushback all of plates


     string inputline;
     string header;
       cout << hr()<< b("CURRENTLY ACTIVE EXPERIMENTS:") << br() << h2("select a checkbox to stop an experiment")<< br()<< endl;
      cout << "<div class=\"task-list\">" << endl;
	cout << " <script type=\"text/javascript\" src=\"http://code.jquery.com/jquery-latest.min.js\" /></script>" << endl;
	cout << "<script type=\"text/javascript\"> " << endl;
	cout << "function toggle(source) {var checkboxes = document.querySelectorAll('input[name^=\"v\"]');for (var i = 0; i < checkboxes.length; i++) { if (checkboxes[i] != source) checkboxes[i].checked = source.checked;    }   var checkboxen = document.querySelectorAll('input[name^=\"del\"]');for (var i = 0; i < checkboxen.length; i++) { if (checkboxen[i] != source) checkboxen[i].checked = source.checked;    }   }" << endl;
	cout << "</script>" << endl;
	cout << "<table id=\"task-table\">" << endl;
	cout <<  "<tr><th>ExpID</th><th>Plate#</th><th>Well#</th><th>Investigator</th><th>Title</th><th>Link</th><th>Select</th><th>Verify</th><th>Frame#</tr>" << endl;
 	
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
	 string currentframe;
	 string epoch;
    	 string directory;
    	 string wellname;
    	 string activeTimelapse;
     	 string activeDailyMonitor;
	 string activeCherry;
	 string activeUV;
	 string activeGFP;
	 string strain;
	 string startingN;
	 string startingAge;
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
				getline(ss, activeUV, ',');
				getline(ss, activeGFP, ',');
				getline(ss, activeCherry, ',');
     				 getline(ss, email, ',');
     				 getline(ss, investigator, ',');
     				 getline(ss, title, ',');
     				 getline(ss, description, ',');
				getline(ss, startingN, ',');
				getline(ss, startingAge, ',');
			        getline(ss, strain, ',');
			        getline(ss, currentframe, ',');
				getline(ss, epoch, ',');

				

     //				 cout << "status" << status << endl;
     				 if (status){
     					 platesinuse.push_back(plate);
     					 stringstream  expValue;
     					 expValue << expID;

     					 cout << "<tr>" << endl;
     					 cout << "<th>" << expID << "</th>" << endl;
     					 cout << "<th>" << plate << "</th>" << endl;
     					 cout << "<th>" << wellname << "</th>" << endl;
     					 cout << "<th>" << investigator << "</th>" << endl;
     					 cout << "<th>" << title << " " << description << "</th>" << endl;
     					 cout << "<th>" << "<a href=\"/cgi-bin/marker?loadedexpID=" << expID <<"\" > ANALYZE </a>" << "</th>" << endl;
     					 cout << "<th><input type=\"checkbox\" name=\"del" << expValue.str().c_str()<<"\"></th>" << endl;
     					 cout << "<th><input type=\"checkbox\" name=\"v" << expValue.str().c_str()<<"\"></th>" << endl;
     					 cout << "<th hidden disabled>" << email << "</th>" << endl;
     					 cout << "<th hidden disabled>" << title << "</th>" << endl;
     					 cout << "<th hidden disabled>" << description << "</th>" << endl;
     					 cout << "<th hidden disabled>" << strain << "</th>" << endl;
     					 cout << "<th hidden disabled>" << startingN << "</th>" << endl;
     					 cout << "<th hidden disabled>" << startingAge << "</th>" << endl;
					cout << "<th hidden disabled>" << epoch << "</th>" << endl;
					cout << "<th>" << currentframe << "</th>" << endl;
					cout << "<th hidden disabled>" << activeUV << "</th>" << endl;
					cout << "<th hidden disabled>" << activeGFP << "</th>" << endl;
					cout << "<th hidden disabled>" << activeCherry << "</th>" << endl;
				 	cout << "<th hidden disabled>" << activeTimelapse << "</th>" << endl;
					cout << "<th hidden disabled>" << activeDailyMonitor << "</th>" << endl;
     					 cout << "</tr>" << endl;
     				 }//end if active plate

     }//end while lines in file
     cout << "</table>" << endl;
		cout << "<input type=\"checkbox\" onclick=\"toggle(this);\" />Select All to Delete<br /> " << endl;
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
		string activeGFP;
		string activeCherry;
		string activeUV;
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
					getline(ss, activeUV, ',');
					 getline(ss, activeGFP, ',');
					getline(ss,activeCherry, ',');
					 getline(ss, email, ',');
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
	    	  sss << "del" << (*citer);
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
		string dailymonitorcheck,gfpCheck,cherryCheck,uvCheck;
		dailymonitorcheck=string("adm") + boost::lexical_cast<string>(i);
		gfpCheck=string("agfp") + boost::lexical_cast<string>(i); 
		cherryCheck=string("acherry") + boost::lexical_cast<string>(i);
		uvCheck=string("auv") + boost::lexical_cast<string>(i);

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
			//get exposure times			
			oss << atol(cgi(uvCheck).c_str()) << ",";
			oss << atol(cgi(gfpCheck).c_str()) << ",";
			oss << atol(cgi(cherryCheck).c_str()) << ",";
			/*
			if (cgi.queryCheckbox(uvCheck)) oss << UV_ON << ","; else oss << UV_OFF << ",";
			if (cgi.queryCheckbox(gfpCheck)) oss << GFP_ON << ","; else oss << GFP_OFF << ",";
			if (cgi.queryCheckbox(cherryCheck)) oss << CHERRY_ON << ","; else oss << CHERRY_OFF << ",";
			*/


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

string buildInputField(vector<int> openplates){
	stringstream ss;

	ss << "<select name=\"plate\" id=\"select-plate\" > ";  // editing old data broken// onchange=fillSelectedPlateWithOldData(this.value)
	 for (vector<int>::iterator citer = openplates.begin(); citer != openplates.end(); citer++){
	           ss << "<option name=\"" << *citer << "\" > " << *citer << "</option>";
	}//end for openplates

	// for now, hard code 12 plates into it
	//for (int i = 1; i <= 12; i++) {
	//	ss << "<option name=\"" << i << "\" > " << i << "</option>";
	//}
	ss << "</select>";

	return(ss.str());

}//end buildInputFields



int main(int argc, char **argv) {
	try {

		ifstream readpath("data_path");
		readpath >> datapath;
		// Send HTTP header
		cout << HTTPHTMLHeader() << endl;

		// Set up the HTML document
		cout << html() << head(title("WormBot Scheduler")) << endl;
		cout << body() << endl;

		//cout << img().set("src","http://kaeberleinlab.org/images/kaeberlein-lab-logo-2.png") << endl;
		loadCss();
		string temp;

		string component_path = datapath + "scheduler_parts/"; // TODO: this need to be the path where you put scheduler_components folder

		// ifstream jsStyleStream(datapath + "scheduler_parts/scheduler.js");
		ifstream jsStyleStream(component_path + "scheduler.js");
		cout << "<script>" << endl;
		while (getline(jsStyleStream, temp)) {
			temp += "\n";
			cout << temp << endl;
		}
		cout << "</script>" << endl;
		ifstream sidebarStream(component_path + "navbar.html");
		while (getline(sidebarStream, temp)) {
			temp += "\n";
			cout << temp << endl;
		}
    	cout << "<div id=\"main\">" << endl;
        // cout << "<span id=\"btn-sidebar\" class=\"btn-sidebar\" onclick=\"controlSideBar()\">&#60;</span>" << endl;
		cout << "<h3><a><span id=\"btn-sidebar\" class=\"btn-sidebar\" onclick=\"controlSideBar()\">&#60;</span></a> SCHEDULER</h3>";
		cout << form().set("action", "/cgi-bin/scheduler").set("method", "POST");

		processDeletes();
		processInput();
		//sortJobList();


		vector<int> openplates;
		openplates = readOpenPlates();
		cout << hr() << endl;
		cout << "<span id=\"open-plates-display\" style=\"margin-top:10%\">";
		for (vector<int>::iterator citer = openplates.begin(); citer != openplates.end(); citer++){
			if (citer + 1 == openplates.end()) {
				cout << (*citer);
			} else {
				cout << (*citer) << ", ";
			}
		}
		cout << "</span>" << endl;
		cout << br() <<endl;

		cout << h2("Add a plate") << br() << "Select plate slot: " << endl;

		cout << buildInputField(openplates) << endl;

		cout << "<button type=\"button\" id=\"myBtn\">Open Template</button>" << endl;
		// ifstream modalStream(datapath + "scheduler_parts/modal.html");
		ifstream modalStream(component_path + "modal.html");
		while (getline(modalStream, temp)) {
			temp += "\n";
			cout << temp << endl;
		}

		string plateformloc;
		plateformloc = string(datapath + "plateform.html");
		ifstream plateform(plateformloc.c_str());
		string formline;
		stringstream ss;

		cout << "<div class=\"plate-list\">" << endl;

		while (getline(plateform,formline)){
			ss << formline;
        }
    	for (int i=0; i < 12; i++) {
    		string tempr=ss.str();
    		stringstream number;
    		number << i;
    		boost::replace_all(tempr, "XXX", number.str());
    		cout << tempr << hr()<<endl;
        }

		cout << "</div>" << endl;
		cout << "<input type=\"submit\" style=\"float:right;margin: 1% 1% 1% 0%;\" value=\"Update Robot\"> <input type=\"reset\" style=\"float:right;margin: 1% 1% 1% 0%;\" value=\"clear form\">" << endl;
		cout << "</form>" << endl;
		cout << "</div>" << endl;
		cout << "<script>" << "assignDefaultSelectOption();movePlatesDisplay();fillSelectedPlateWithOldData();addAllSlotListeners();" << "</script>" << endl;
		// Close the HTML document
		cout << body() << html();

	} catch(exception& e) {
		// handle any errors - omitted for brevity
	}
}
