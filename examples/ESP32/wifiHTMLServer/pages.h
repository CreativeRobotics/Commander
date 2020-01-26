String htmlStart = "<html>";
String htmlEnd = "</html>";

String helloPage = 
    "<head>"
        "<title>Basic Web Page</title>"
    "</head>"
    "<body>"
"<p style=\"color:olive;font-size:24px;\">Helloo Darcy</p>"
    "</body>";
    
String cmdHeadErrorPage = 
    "<head>"
        "<title>Command Web Page</title>"
    "</head>"
    "<body>"
    "<p><h2>Error - Command Not Recognised</h2></p>"
      "<p><h3>Command list</h3></p>"
      "<pre>";

String cmdHeadPage = 
    "<head>"
        "<title>Command Web Page</title>"
    "</head>"
    "<body>"
    "<p><h2>Commander:</h2></p>"
      "<p><h3>Command list</h3></p>"
      "<pre>";

String cmdFootPage = 
      "</pre>"
    "</body>";

String pageFooter = 
 "<footer>"
  "<p>Commander ESP32 webserver example</p>"
  "<p><a href=\"help\">Help</a></p>"
"</footer>";

String testPage = 
    "<head>"
        "<title>Test Web Page</title>"
    "</head>"
    "<body>"
"<p>This page is a test page</p>"
    "</body>";


String cmdForm = 
"<br>"
 "<form action=\"/submit\">"
  "Command:<br>"
  "<input type=\"text\" name=\"Command\" value=\"help\"><br>"
  "<input type=\"submit\" value=\"Submit\">"
"</form>"
"<br>";



String myHomework = 
"<head>"
"<title>Coding Instructions</title>"
"</head>"
"<body>"
"<body style=\"background-color: lightBlue;\">"
"<body style=\"padding-left:60px;\">"
"<h1>Coding Thingies</h1>"
"<h2>Processing</h2>"
"<p>"
"void - keyword used indicate that a function returns no value. Each function must either return a value of a specific datatype or use the keyword void to specify it returns nothing.<br>"
"() (parenthesis) - Grouping and containing expressions and parameters. Some functions have no parameters and in this case, the space between parentheses is blank.<br>"
", (comma) - Separates parameters in function calls and elements during assignment.<br>"
". (dot) - Provides access to an object's methods and data.<br>"
"/* */ (multiline comment) - Explanatory notes embedded within the code. Comments are used to remind yourself and to inform others about the function of your program.<br>"
"/** */ (doc comment) - Explanatory notes embedded within the code and written to the \"index.html\" file created when the code is exported.<br>"
"// (comment) - single-line comments embedded within the code. These are ignored by the program whilst running.<br>"
"; (semicolon) - A statement ending that separates parts of the program. They do the same job as a full stop in written English.<br>"
"= (assign) - Assigns a value to a variable. Formerly called an assignment operator.<br>"
"[] (array access) - Used to specify a location within an array.<br>"
"{} (curly braces) - Used to define the beggining and end of functions blocks and statement blocks.<br>"
"catch - used with <b>try</b> to handle exceptions.<br>"
"class - used to indicate the declaration of a class (a composite of data and methods)."
"</p>"
"</body>";

String robotsPage = 
"<head>"
"<title>Robots</title>"
"</head>"
"<body>"
"<body style=\"background-color: lightBlue;\">"
"<body style=\"padding-left:60px;\">"
"<h1>All hail our robotic overlords</h1>"
"</body>";
