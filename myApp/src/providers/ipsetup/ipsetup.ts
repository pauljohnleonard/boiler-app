import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { Subject } from "rxjs/Subject"
import { timeout } from 'rxjs/operators/timeout';
// import { Observable } from 'rxjs/Observable';
/*
  Generated class for the IpProvider provider.

  See https://angular.io/guide/dependency-injection for more info on providers
  and Angular DI.
*/


// declare var window: any

// function getLanIP(cb) {
//     // NOTE: window.RTCPeerConnection is "not a constructor" in FF22/23
//     var RTCPeerConnection = /*window.RTCPeerConnection ||*/ window.webkitRTCPeerConnection || window.mozRTCPeerConnection;

//     var rtc = new RTCPeerConnection({ iceServers: [] });

//     if (1 || window.mozRTCPeerConnection) {      // FF [and now Chrome!] needs a channel/stream to proceed
//         rtc.createDataChannel('', { reliable: false });
//     };

//     rtc.onicecandidate = function (evt) {
//         // convert the candidate to SDP so we can run it through our general parser
//         // see https://twitter.com/lancestout/status/525796175425720320 for details
//         if (evt.candidate) grepSDP("a=" + evt.candidate.candidate);
//     };

//     rtc.createOffer(function (offerDesc) {
//         grepSDP(offerDesc.sdp);
//         rtc.setLocalDescription(offerDesc);
//     }, function (e) { console.warn("offer failed", e); });

//     function grepSDP(sdp) {
//         //  var hosts = [];
//         sdp.split('\r\n').forEach(function (line) { // c.f. http://tools.ietf.org/html/rfc4566#page-39
//             if (~line.indexOf("a=candidate")) {     // http://tools.ietf.org/html/rfc4566#section-5.13
//                 let parts = line.split(' '),        // http://tools.ietf.org/html/rfc5245#section-15.1
//                     addr = parts[4],
//                     type = parts[7];
//                 if (type === 'host') cb("A" + line);
//             } else if (~line.indexOf("c=")) {       // http://tools.ietf.org/html/rfc4566#section-5.7
//                 let parts = line.split(' '),
//                     addr = parts[2];
//                 cb("B" + line);
//             }
//         });
//     }

// }

@Injectable()
export class IpSetupProvider {

    ipSubject = new Subject<any>();
    searching = false;

    constructor(public http: HttpClient) {
        console.log('Hello IpProvider Provider');
    }


    searchForDevice(ipbase: string) {
        
        this.searching = true;

        for (var i = 0; i < 255; i++) {

            const url = "http://" + ipbase + "." + i + "/";
            const urlCmd = url + "?cmd=ping";
         
            try {
                this.http.get(urlCmd, { responseType: 'text' }).pipe(timeout(5000)).subscribe(
                    (res) => {
                        console.log(url + " --> " + res);
                        this.ipSubject.next(url);
                        this.searching = false;
                    },
                    (err) => {
                        // console.log(err);
                    })
            } catch (err) {

            }
        }

    }

}
