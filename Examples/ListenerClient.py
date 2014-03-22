'''
Copyright (c) <2012> Tarek Galal <tare2.galal@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this 
software and associated documentation files (the "Software"), to deal in the Software 
without restriction, including without limitation the rights to use, copy, modify, 
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
permit persons to whom the Software is furnished to do so, subject to the following 
conditions:

The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF 
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
'''

import os
parentdir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.sys.path.insert(0,parentdir)
import datetime, sys, time

if sys.version_info >= (3, 0):
	raw_input = input

from Yowsup.connectionmanager import YowsupConnectionManager

class WhatsappListenerClient:
	
	def __init__(self, keepAlive = False, sendReceipts = False, waitForReceipt = False):
		self.sendReceipts = sendReceipts
		self.waitForReceipt = waitForReceipt
		
		connectionManager = YowsupConnectionManager()
		connectionManager.setAutoPong(keepAlive)

		self.signalsInterface = connectionManager.getSignalsInterface()
		self.methodsInterface = connectionManager.getMethodsInterface()

		if waitForReceipt:
			self.signalsInterface.registerListener("receipt_messageSent", self.onMessageSent)
			self.gotReceipt = False
		
		self.signalsInterface.registerListener("message_received", self.onMessageReceived)
		self.signalsInterface.registerListener("group_messageReceived", self.onGroupMessageReceived)
		self.signalsInterface.registerListener("auth_success", self.onAuthSuccess)
		self.signalsInterface.registerListener("auth_fail", self.onAuthFailed)
		self.signalsInterface.registerListener("disconnected", self.onDisconnected)
		
		self.cm = connectionManager
	
	def login(self, username, password):
		self.username = username
		self.methodsInterface.call("auth_login", (username, password))

		while True:
			raw_input()

	def send(self, target, message, waitForReceipt=False):

		self.jids = []
		
		if '-' in target:
			self.jids = ["%s@g.us" % target]
		else:
			self.jids = ["%s@s.whatsapp.net" % t for t in target.split(',')]

		self.message = message.encode('utf8')
		self.waitForReceipt = waitForReceipt

		if self.waitForReceipt:
			self.methodsInterface.call("ready")
		
		
		if len(self.jids) > 1:
			self.methodsInterface.call("message_broadcast", (self.jids, self.message))
		else:
			self.methodsInterface.call("message_send", (self.jids[0], self.message))
		print("Sent message")
		if self.waitForReceipt:
			timeout = 5
			t = 0;
			while t < timeout and not self.gotReceipt:
				time.sleep(0.5)
				t+=1

			if not self.gotReceipt:
				print("print timedout!")
			else:
				print("Got sent receipt")

		self.done = True		

	def onAuthSuccess(self, username):
		print("Authed %s" % username)
		self.methodsInterface.call("ready")

	def onAuthFailed(self, username, err):
		print("Auth Failed!")

	def onDisconnected(self, reason):
		print("Disconnected because %s" %reason)

	def onMessageReceived(self, messageId, jid, messageContent, timestamp, wantsReceipt, pushName, isBroadCast):
		formattedDate = datetime.datetime.fromtimestamp(timestamp).strftime('%d-%m-%Y %H:%M')
		print("%s [%s]:%s"%(jid, formattedDate, messageContent))

		if wantsReceipt and self.sendReceipts:
			self.methodsInterface.call("message_ack", (jid, messageId))

		import urllib2
		import urllib
		params = { 'number' : jid, 'message' : messageContent, 'message_id': messageId}
		response = urllib2.urlopen("http://localhost:3000/api/whatsapp/on_message_received?" + urllib.urlencode(params))

	def onMessageSent(self, jid, messageId):
		self.gotReceipt = True
	
	def onGroupMessageReceived(self, messageId, jid, author, content, timestamp, receiptRequested, extra):
		formattedDate = datetime.datetime.fromtimestamp(timestamp).strftime('%d-%m-%Y %H:%M')
		print("group %s [%s]:%s"%(jid, formattedDate, author))

		if receiptRequested and self.sendReceipts:
			self.methodsInterface.call("message_ack", (jid, messageId))

		import urllib2
		import urllib
		params = { 'number' : jid, 'message' : messageId}
		response = urllib2.urlopen("http://localhost:3000/api/whatsapp/on_group_message_received?" + urllib.urlencode(params))
