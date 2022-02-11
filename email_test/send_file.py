##------------------------------------------
##--- Author: Pradeep Singh
##--- Blog: https://iotbytes.wordpress.com/programmatically-send-e-mail-from-raspberry-pi-using-python-and-gmail/
##--- Date: 21st Feb 2017
##--- Version: 1.0
##--- Python Ver: 2.7
##------------------------------------------
from sys import exit, argv, stderr, stdout
import re
import argparse

def getText():
    parser = argparse.ArgumentParser(description="Send_file")
    parser.add_argument('infile', nargs=1)
    parser.add_argument('subject', nargs='?')

    args = parser.parse_args()

    f = open(args.infile[0],'r')
    text = f.read()
    f.close()
    try:
        subj = args.subject
    except:
        subj = "Mystery"
    return text, subj




#import the class definition from "email_handler.py" file
from email_handler import Class_eMail

msg,subj = getText()

print "subj ["+subj+"]"
print "msg ["+msg+"]"
#exit(0)

#set the email ID where you want to send the test email
To_Email_ID = "sysdcs@gmail.com"

# Send Plain Text Email
email = Class_eMail()
email.send_Text_Mail(To_Email_ID, subj, msg)
del email

# Send HTML Email
#email = Class_eMail()
#email.send_HTML_Mail(To_Email_ID, 'HTML Mail Subject', '<html><h1>This is sample HTML test email body</h1></html>')
#del email
