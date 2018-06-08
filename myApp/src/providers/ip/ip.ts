import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { Subject} from "rxjs/Subject"
/*
  Generated class for the IpProvider provider.

  See https://angular.io/guide/dependency-injection for more info on providers
  and Angular DI.
*/



function getLanIP(cb) {
  // NOTE: window.RTCPeerConnection is "not a constructor" in FF22/23
   var RTCPeerConnection = /*window.RTCPeerConnection ||*/ window.webkitRTCPeerConnection || window.mozRTCPeerConnection;

    var rtc = new RTCPeerConnection({iceServers:[]});

    if (1 || window.mozRTCPeerConnection) {      // FF [and now Chrome!] needs a channel/stream to proceed
        rtc.createDataChannel('', {reliable:false});
    };
    
    rtc.onicecandidate = function (evt) {
        // convert the candidate to SDP so we can run it through our general parser
        // see https://twitter.com/lancestout/status/525796175425720320 for details
        if (evt.candidate) grepSDP("a="+evt.candidate.candidate);
    };

    rtc.createOffer(function (offerDesc) {
        grepSDP(offerDesc.sdp);
        rtc.setLocalDescription(offerDesc);
    }, function (e) { console.warn("offer failed", e); });
      
    function grepSDP(sdp) {
      //  var hosts = [];
        sdp.split('\r\n').forEach(function (line) { // c.f. http://tools.ietf.org/html/rfc4566#page-39
            if (~line.indexOf("a=candidate")) {     // http://tools.ietf.org/html/rfc4566#section-5.13
                let parts = line.split(' '),        // http://tools.ietf.org/html/rfc5245#section-15.1
                    addr = parts[4],
                    type = parts[7];
                if (type === 'host') cb(addr);
            } else if (~line.indexOf("c=")) {       // http://tools.ietf.org/html/rfc4566#section-5.7
                let parts = line.split(' '),
                    addr = parts[2];
                cb(addr);
            }
        });
    }

}

@Injectable()
export class IpProvider {

  ipSubject= new Subject<any>(); 
  constructor(public http: HttpClient) {

    getLanIP( (val) => {
      this.ipSubject.next({ip:val});
    });
    console.log('Hello IpProvider Provider');
  }

}
