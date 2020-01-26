//web page utilities for Commander

//Returns a string containing the command from a GET request

String GET_CommandString(String serverGetLine, String defaultString){
	int startOf = serverGetLine.indexOf("GET /");
	int endOf = serverGetLine.indexOf(" HTTP");
	if(startOf == -1) return ""; //return an empty string if GET is not in the line
	
	startOf += 5; //startOf now corresponds to the space after the slash
	if(startOf == endOf) return  defaultString; //return the default string if there is nothing found
	int equalsIndex = serverGetLine.indexOf("=");
	if(equalsIndex > -1) startOf = equalsIndex+1; //if it was a form submission, get the submitted text
	
	
	String returnString = serverGetLine.substring(startOf, endOf);
	returnString.replace("%3F", "?"); //replace special characters
	for(int n = 0; n < returnString.length(); n++){
		if(returnString.charAt(n) == '+' || returnString.charAt(n) == '/') returnString.setCharAt(n, ' ');
	}
	return returnString;
}
