import base64

line = raw_input()
data =line

# S3cur3K3y

def xor_crypt_string(data, key = "S3cur3K3yS3cur3K3yS3cur3K3y", encode = False, decode = True):
   from itertools import izip, cycle
   import base64
   
   if decode:
      data = base64.decodestring(data)
   xored = ''.join(chr(ord(x) ^ ord(y)) for (x,y) in izip(data, cycle(key)))
   
   if encode:
      return base64.encodestring(xored).strip()
   return xored
secret_data = "XOR procedure"


print xor_crypt_string(data, encode = False)
