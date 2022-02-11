"""
https://codehandbook.org/how-to-read-email-from-gmail-using-python/

"""
import smtplib
import time
import imaplib
import email

ORG_EMAIL   = "@gmail.com"
FROM_EMAIL  = "sdcsiot" + ORG_EMAIL
JUNK        ="lovt"
FROM_PWD    = "eegvbsaznjzh"
SMTP_SERVER = "imap.gmail.com"
SMTP_PORT   = 993


# -------------------------------------------------
#
# Utility to read email from Gmail Using Python
#
# ------------------------------------------------
def read_email_from_gmail():
    try:
        mail = imaplib.IMAP4_SSL(SMTP_SERVER)
        mail.login(FROM_EMAIL,JUNK+FROM_PWD)
        mail.select('inbox')
        
        type, data = mail.search(None, 'ALL')
        mail_ids = data[0]
        
        id_list = mail_ids.split()
        first_email_id = int(id_list[0])
        latest_email_id = int(id_list[-1])

        for i in range(latest_email_id,first_email_id, -1):
            typ, data = mail.fetch(i, '(RFC822)' )

            for response_part in data:
                if isinstance(response_part, tuple):
                    msg = email.message_from_string(response_part[1])
                    email_subject = msg['subject']                    
                    email_from = msg['from']
                    #print 'From : ' + email_from + '\n'
                    print 'ID: ' + str(i) 
                    print 'From : ' + email_from 
                    print 'Subject : ' + email_subject + '\n'

    except Exception, e:
        print str(e)

if __name__ == "__main__":
    read_email_from_gmail()
