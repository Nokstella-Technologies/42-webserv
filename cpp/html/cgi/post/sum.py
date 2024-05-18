import os
import sys
import warnings
warnings.filterwarnings("ignore", category=DeprecationWarning)
import json

def sum():
	try:
		json_data = os.environ["REQUEST_BODY"]
		data = (json.loads(json_data))
		num1 = data.get("num1","error1")
		num2 = data.get("num2","error2")
		
		result = float(num1) + float(num2)

		# while 1 == 1:
		# 	a = 1

		return ("{\"result\":" + str(result) + "}")
	except:
		return "{\"Error\": \"Invalid value\"}"

print(sum())
