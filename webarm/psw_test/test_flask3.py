from flask import Flask, request



app = Flask(__name__)


@app.route('/', methods=['GET', 'POST'])

def index():

        print("Posted data : {}".format(request.form))
        return """
<form method="post">
<input type="text" name="txt1" id="idtxt1">
<input type="text" name="txt2" id="idtxt2" value="box2">
<input type="text" name="txt3" id="idtxt3" value="box3">  
<input type="submit" Value="Send">
</form>
        """


    
if __name__ == "__main__":
    app.run(host='0.0.0.0', port=5001, debug=True)
        
