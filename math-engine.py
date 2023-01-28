import json

line = input()
data = json.loads(line)

#print(data)

equation = [0, 0]
operation = ""

for val in data["operators"]:
  if val["type"] == "number":
    pos = int(val["position"])
    equation[0 if pos == 0 else pos - 1] = str(val["content"])
  if val["type"] == "string":
    operation = val["content"]

if(operation == "+"):
  print(int(equation[0]) + int(equation[1]))
elif(operation == "-"):
  print(int(equation[0]) - int(equation[1]))
elif(operation == "/"):
  print(int(int(equation[0]) / int(equation[1])))
elif(operation == "*"):
  print(int(equation[0]) * int(equation[1]))
