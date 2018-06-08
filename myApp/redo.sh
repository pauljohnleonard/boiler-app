LIST='cordova-plugin-ionic-keyboard
cordova-plugin-networkinterface
cordova-plugin-whitelist
cordova-plugin-device
cordova-plugin-ionic-webview
cordova-plugin-splashscreen' 

for i in ${LIST}
do
ionic cordova plugin remove $i
done



for i in ${LIST}
do
ionic cordova plugin add $i
done
