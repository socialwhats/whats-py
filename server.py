import web, os
import threading,time, base64

from Examples.ListenerClient import WhatsappListenerClient
from Yowsup.Common.utilities import Utilities

DEFAULT_CONFIG = "config"
wa = WhatsappListenerClient(True, True, True)

def getCredentials(config = DEFAULT_CONFIG):
	if os.path.isfile(config):
		f = open(config)
	
		phone = ""
		idx = ""
		pw = ""
		cc = ""
	
		try:
			for l in f:
				line = l.strip()
				if len(line) and line[0] not in ('#',';'):
					
					prep = line.split('#', 1)[0].split(';', 1)[0].split('=', 1)
					
					varname = prep[0].strip()
					val = prep[1].strip()
					
					if varname == "phone":
						phone = val
					elif varname == "id":
						idx = val
					elif varname =="password":
						pw =val
					elif varname == "cc":
						cc = val

			return (cc, phone, idx, pw);
		except:
			pass

	return 0

countryCode, login, identity, password = getCredentials()

identity = Utilities.processIdentity(identity)
password = base64.b64decode(bytes(password.encode('utf-8')))

if countryCode:
	phoneNumber = login[len(countryCode):]
else:
	dissected = dissectPhoneNumber(login)
	if not dissected:
			sys.exit("ERROR. Couldn't detect cc, you have to manually place it your config")
	countryCode, phoneNumber = dissected

def start_listening():
	# listen to new messages
	wa.login(login, password)

class Hooks(threading.Thread):

	def run (self):

		urls = ('/.*', 'Hooks')
		app = web.application(urls, globals())
		app.run()

	def GET(self):

		request_data = web.input(action="send", dest="", message="", jid="")
		action = request_data.action

		if action == "send":
			dest = request_data.dest
			message = request_data.message
			wa.send(dest, message, True)
			return "OK"

		elif action =="group_info":
			jid = request_data.jid
			wa.getGroupParticipants(jid);
			return "OK"


Hooks().start()
start_listening()