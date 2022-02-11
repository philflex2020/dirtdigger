#!/usr/bin/python

from flask import Flask



gpiodoc =(\
    "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}" 
    +".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;"
    +"text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}"
    +".button2 {background-color: #555555;}</style>"
    +"<h1> Pump Status </h1>"
     +"<p>GPIO 26 - State OFF </p>"
     +"<p><a href='/gpio/26/on'><button class='button'>ON</button></a></p>"
     +"<p>GPIO 27 - State ON </p>"
     +"<p><a href='/gpio/27/off'><button class='button button2'>OFF</button></a></p>"
     +"<div id='demo'> <h2>The XMLHttpRequest Object</h2>"
     +"<button type='button' onclick='loadDoc()'>Change Content</button>"
    +"</div>"
     +"<script> setInterval (loadDoc, 5000) ; function loadDoc() { var xhttp = new XMLHttpRequest();"
     +"xhttp.onreadystatechange = function() { "
     +"if (this.readyState == 4 && this.status == 200) {"
     + "document.getElementById('demo').innerHTML ="
     + "this.responseText;}};"
     +"xhttp.open('GET', 'ajax_info.txt', true);"
     +"xhttp.send();} </script>"
    +"<form name='loginForm'>"
    +"<table width='20%' bgcolor='A09F9F' align='center'>"
     +   "<tr>"
      +      "<td colspan=2>"
       +         "<center><font size=4><b>Pump Control Page</b></font></center>"
       +         "<br>"
       +     "</td>"
       +     "<br>"
       +     "<br>"
       + "</tr>"
       + "<td>Gpios:</td>"
       + "<td><input type='text' size=25 name='userid'><br><a href='/gpio/26'> 26</a></td>"
        +"</tr>"
        +"<br>"
        +"<br>"
        +"<tr>"
        +    "<td>Password:</td>"
        +    "<td><input type='Password' size=25 name='pwd'><br></td>"
        +    "<br>"
        +    "<br>"
        +"</tr>"
        +"<tr>"
        +    "<td><input type='submit' onclick='check(this.form)' value='Login'></td>"
        +"</tr>"
    +"</table>"
    +"</form>"
    +"<script>"
    +"function check(form)"
    +"{"
    +"if(form.userid.value=='admin' && form.pwd.value=='admin')"
    +"{"
    +"window.open('/serverIndex')"
    +"}"
    +"else"
    +"{"
    +" alert('Error Password or Username')/*displays error message*/"
    +"}"
    +"}"
    +"</script>"
)


#htmldoc =(
#+ "<!DOCTYPE html>
#<html lang="en">
 
#<head>
#    <title>Python Flask Bucket List App</title>
 
 
#    <link href="http://getbootstrap.com/dist/css/bootstrap.min.css" rel="stylesheet">
 
#    <link href="http://getbootstrap.com/examples/jumbotron-narrow/jumbotron-narrow.css" rel="stylesheet">
 
 
#</head>
 
#<body>
 
#    <div class="container">
#        <div class="header">
#            <nav>
#                <ul class="nav nav-pills pull-right">
#                    <li role="presentation" class="active"><a href="#">Home</a>
#                    </li>
#                    <li role="presentation"><a href="#">Sign In</a>
#                    </li>
#                    <li role="presentation"><a href="showSignUp">Sign Up</a>
#                    </li>
#                </ul>
#            </nav>
#            <h3 class="text-muted">Python Flask App</h3>
#        </div>
 
#        <div class="jumbotron">
#            <h1>Bucket List App</h1>
#            <p class="lead"></p>
#            <p><a class="btn btn-lg btn-success" href="showSignUp" role="button">Sign up today</a>
#            </p>
#        </div>
 
#        <div class="row marketing">
#            <div class="col-lg-6">
#                <h4>Bucket List</h4>
#                <p>Donec id elit non mi porta gravida at eget metus. Maecenas faucibus mollis interdum.</p>
 
#                <h4>Bucket List</h4>
#                <p>Morbi leo risus, porta ac consectetur ac, vestibulum at eros. Cras mattis consectetur purus sit amet fermentum.</p>
 
#                <h4>Bucket List</h4>
#                <p>Maecenas sed diam eget risus varius blandit sit amet non magna.</p>
#            </div>
 
#            <div class="col-lg-6">
#                <h4>Bucket List</h4>
#                <p>Donec id elit non mi porta gravida at eget metus. Maecenas faucibus mollis interdum.</p>
 
 #               <h4>Bucket List</h4>
 #               <p>Morbi leo risus, porta ac consectetur ac, vestibulum at eros. Cras mattis consectetur purus sit amet fermentum.</p>
 
 #               <h4>Bucket List</h4>
 #               <p>Maecenas sed diam eget risus varius blandit sit amet non magna.</p>
 #           </div>
 #       </div>
 
 #       <footer class="footer">
 #           <p>&copy; Company 2015</p>
 #       </footer>
 
 #   </div>
#</body>
 
#</html>



app = Flask(__name__)
#Now define the basic route / and its corresponding request handler:

@app.route("/")
def main():
    #return "Welcome!"
    return gpiodoc

#Next, check if the executed file is the main program and run the app:



if __name__ == "__main__":
    app.run(host='0.0.0.0', port=8000)

    #python app.py
#Point your browser to http://localhost:5000/ and you should have the welcome message.

#Creating a Home Page
#First, when the application runs we should show a home page with the latest bucket list items added by users. So let's add our home page to our application folder.

#Flask looks for template files inside the templates folder. So navigate to the PythonApp folder and create a folder called templates. Inside templates, create a file called index.html. Open up index.html and add the following HTML:

#Open up app.py and import render_template, which we'll use to render the template files.

#from flask import Flask, render_template
#Modify the main method to return the rendered template file.

#def main():
#    return render_template('index.html')
