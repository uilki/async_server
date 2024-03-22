#Generate a private key

openssl genrsa -des3 -passout pass:test -out server.key 2048 
echo "Private key generated"

#Generate Certificate signing request
openssl req -new -key server.key -passin pass:test -subj "/C=UA/ST=Kiyv/L=Kiyv/O=Home/CN=home/emailAddress=name@example.com" -out server.csr
echo "Signing request generated"

#Sign certificate with private key
openssl x509 -req -passin pass:test -days 3650 -in server.csr -signkey server.key -out server.crt
echo "Certificate signed"

#Remove password requirement (needed for example)
cp server.key server.key.secure
openssl rsa -passin pass:test -in server.key.secure -out server.key
echo "Password removed"

#Generate dhparam file
openssl dhparam -out dh2048.pem 2048
echo "dhparam file generated"

