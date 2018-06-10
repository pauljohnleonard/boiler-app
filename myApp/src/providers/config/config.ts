import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { NetworkInterface } from '@ionic-native/network-interface';
import { Platform } from 'ionic-angular';
import { Observable } from 'rxjs/Observable';
import 'rxjs/add/observable/of';
import { IpSetupProvider } from '../ipsetup/ipsetup';

// import { mergeMap, map, mergeAll, take, bufferCount, concat, merge, concatAll, concatMap, delay } from 'rxjs/operators';

/*
  Generated class for the ConfigProvider provider.

  See https://angular.io/guide/dependency-injection for more info on providers
  and Angular DI.
*/




@Injectable()
export class ConfigProvider {

    public wifiSubject: Observable<any>;
    public lanIPbase = "192.168.0";

    constructor(public http: HttpClient, networkInterface: NetworkInterface, public plt: Platform, ip: IpSetupProvider) {

        console.log('Hello ConfigProvider Provider');
        let IP = window.localStorage.getItem("lanIPbase");
        if (IP) this.lanIPbase = IP;

        if (plt.is('cordova')) {
            this.wifiSubject = Observable.of(networkInterface.getWiFiIPAddress());
        } else {
            this.wifiSubject = ip.ipSubject;   // Observable.of( {ip:"192.168.0.1",mask:"255.255.255.0"});
        }
    }
}


