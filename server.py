import web, os
import threading,time, base64

from Examples.EchoClient import WhatsappEchoClient
from Yowsup.Common.utilities import Utilities

urls = ('/.*', 'hooks')
app = web.application(urls, globals())

DEFAULT_CONFIG = "config"

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

class hooks:
	def GET(self):
		request_data = web.input(phone="", message="")

		phone = request_data.phone
		message = request_data.message

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

		wa = WhatsappEchoClient(phone, message.encode("utf-8"), False)
		wa.login(login, password)

if __name__ == '__main__':
	app.run()