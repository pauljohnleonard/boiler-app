import { ConfigProvider } from '../../providers/config/config';
import { Component } from '@angular/core';
import { NavController } from 'ionic-angular';

@Component({
    selector: 'page-about',
    templateUrl: 'about.html'
})
export class AboutPage {

    wifiIP: string ="UNSET";
    constructor(public navCtrl: NavController, config: ConfigProvider) {
        console.log('Hello ConfigProvider Provider');

        config.wifiSubject.subscribe((ip) => {
            this.wifiIP = JSON.stringify(ip, null, 2)
        });


        // networkInterface.getCarrierIPAddress().then((ip) => {
        //     this.carrierIP = ip
        // });
    }





}
