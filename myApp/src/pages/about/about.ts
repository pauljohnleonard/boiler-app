import {take} from 'rxjs/operators/take';
import { ConfigProvider } from '../../providers/config/config';
import { Component } from '@angular/core';
import { NavController } from 'ionic-angular';
import { IpSetupProvider } from '../../providers/ipsetup/ipsetup';

@Component({
    selector: 'page-about',
    templateUrl: 'about.html'
})
export class AboutPage {

    wifiIPsuggest: string ="UNSET";
   

    constructor(public navCtrl: NavController,public config: ConfigProvider,public ip:IpSetupProvider) {
        console.log('Hello ConfigProvider Provider');

        config.wifiSubject.subscribe((ip) => {
            this.wifiIPsuggest += ip+"\n"
        });

        // networkInterface.getCarrierIPAddress().then((ip) => {
        //     this.carrierIP = ip
        // });
    }

    search() {

        let toks = this.config.lanIPbase.split(".");
        if (toks.length > 2 ) {
            this.config.lanIPbase =toks[0]+"."+toks[1]+"."+toks[2]
        } 
        
        this.ip.searchForDevice(this.config.lanIPbase);
 
    }
}
