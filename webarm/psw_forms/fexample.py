from flask import Flask, render_template, request, flash
from forms import ContactForm
app = Flask(__name__)
app.secret_key = 'development key'


@app.route('/contact2', methods = ['GET', 'POST'])
def contact2():
   form = ContactForm()
   
   if request.method == 'POST':
      if form.validate() == False:
         flash('All fields are required.')
         return render_template('contact2.html', form = form)
      else:
         return render_template('success.html')
   elif request.method == 'GET':
       return render_template('contact2.html', form = form)

if __name__ == '__main__':
   app.run(debug = True)
   
