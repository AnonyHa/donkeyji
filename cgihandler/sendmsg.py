#-*- coding: GB2312 -*-
#$Id: sendmsg.py 21270 2007-04-25 10:49:03Z zhuang $

import urllib, smtplib
import mimetypes
from email import Encoders
from email.MIMEBase import MIMEBase
from email.MIMEText import MIMEText
from email.MIMEMultipart import MIMEMultipart

class LocalSendMail(smtplib.SMTP):
	def __init__(self):
		self.host = "localhost"
		smtplib.SMTP.__init__(self, self.host)


class SendMail(LocalSendMail):
	def __init__(self):
		LocalSendMail.__init__(self)
		self.mailfrom = None
		self.mailto = None

	def setfrom(self, mailfrom):
		self.mailfrom = mailfrom

	def setto(self, mailto):
		self.mailto = mailto

	def sendmail(self, to_addrs_list, subject, msg):
		header = ""
		if self.mailfrom != None:
			header = "From: " + str(self.mailfrom) + "\n"
		if self.mailto != None:
			header += "To:  " + self.mailto + "\n"
		header += "Subject: " + subject + "\n\n\n"
		allmsg = header + msg
		return LocalSendMail.sendmail(self, self.mailfrom, to_addrs_list, allmsg)


	#from Python cookbook p340
	def MIMEhtml(self, subject, html):
		import MimeWriter
		import mimetools
		import cStringIO

		htmlin = cStringIO.StringIO(html)
		out = cStringIO.StringIO()

		writer = MimeWriter.MimeWriter(out)
		if self.mailto != None:
			writer.addheader("To", self.mailto)
		if self.mailfrom != None:
			writer.addheader("From", self.mailfrom)
		writer.addheader("Subject", subject)
		writer.addheader("MIME-Version","1.0")

		writer.startmultipartbody("alternative")
		writer.flushheaders()

		subpart = writer.nextpart()
		subpart.addheader("Content-Transfer-Encoding","quoted-printable")
		pout = subpart.startbody("text/html",[("charset","GB2312")])
		mimetools.encode(htmlin, pout, 'quoted-printable')
		htmlin.close()

		writer.lastpart()
		msg = out.getvalue()
		out.close()

		return msg

	def sendMIMEmail(self, to_addrs_list, subject, msg):
		return LocalSendMail.sendmail(self, self.mailfrom, to_addrs_list, self.MIMEhtml(subject, msg))


#moblilelist  example as [13712345678, 13600001234]
def SendMobileNote(mobilelist, msg):
	if len(mobilelist)==0 or len(msg)==0:
		return False
	mobiles = ''
	hMessage = ''

	for i in xrange(0, len(msg)):
		TempChar = msg[i:i+1]
		tChar =str(hex(ord(TempChar)))
		hMessage = hMessage +tChar[2:4]

	for j in xrange(0, len(mobilelist)):
		mobiles += str(mobilelist[j])
		if j != len(mobilelist) - 1:
			mobiles += "|"

	RequestDict = {'phone':mobiles, 'message':hMessage, 'frmphone':'1630', 'msgprop':'10104'}
	Request = urllib.urlencode(RequestDict)

	fp = urllib.urlopen('Http://smsknl.163.com:8089/servlet/SmsIdentifyNotCheck?', Request)
	FileContent = fp.read()
	fp.close()
	sptLine = FileContent.splitlines()
	for line in sptLine:
		if line == 'ok':
			return True
	return  False


def sendmail_withauth(**args):
	"""
	- To     : A list of addresses to send this mail to.  A bare
				string will be treated as a list with 1 address.
	usage example:
		sendmail_withauth(
			From 		= '',
			To 			= [],
			Subject 	= '',
			Content 	= '',
			Attachment  = [],
			Host 		= '',
			Port 		= '',
			User 		= '',
			Password 	= ''
						)
	"""
	Tos = args['To']
	if isinstance(Tos, basestring):       
		Tos = [Tos]

	# create mail obj
	mail_obj = MIMEMultipart()	
	mail_obj['Subject'] = args['Subject']
	mail_obj['From'] = args['From']
	mail_obj['To'] = ",".join(Tos)

	# mail content
	msg = MIMEText(args["Content"] , _charset='gb2312')
	mail_obj.attach(msg)

	# mail attachment
	for filename in args.get("Attachment", []):
		fp = open(filename,'rb')	
		ctype,encoding = mimetypes.guess_type(filename)
		if ctype == None or encoding != None:
			ctype = 'application/octet-stream'
		maintype,subtype = ctype.split('/',1)
		m = MIMEBase(maintype,subtype)
		m.set_payload(fp.read())
		fp.close()
		Encoders.encode_base64(m)
		m.add_header('Content-disposition','attachment',filename=filename.split("/")[-1])
		mail_obj.attach(m)

	sm = smtplib.SMTP(args['Host'], args['Port'])
	# 因163等邮箱对PLAIN认证方法支持不标准，在此做特殊处理
	sm.ehlo()
	sm.esmtp_features["auth"]="LOGIN"
	
	sm.login(args['User'], args['Password'])
	sm.sendmail(args['From'], Tos, mail_obj.as_string())
	sm.quit()