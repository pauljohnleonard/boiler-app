import { IpSetupProvider } from '../ipsetup/ipsetup';
import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { timeout } from 'rxjs/operators/timeout';
import { Subject } from 'rxjs/Subject';

/*
  Generated class for the ModelProvider provider.

  See https://angular.io/guide/dependency-injection for more info on providers
  and Angular DI.
*/


export class Model {

    data:string;

}


@Injectable()
export class ModelProvider {


    subject= new Subject<Model>()
    ip: string;
    model= new Model();


    constructor(public http: HttpClient, public ipSetup: IpSetupProvider) {
        console.log('Hello ModelProvider Provider');
        ipSetup.ipSubject.subscribe((ip) => {
            console.log(" Model got IP", ip);
            this.ip = ip;
            this.getState();
        })

    }


    getState() {
        const url = this.ip + "/state";
        this.http.get(url, { responseType: 'text' }).pipe(timeout(1000)).subscribe(
            (res) => {
                console.log(res);
                this.model.data=res;
                this.subject.next(this.model);
            },
            (err) => {
                console.log(err);
            })
    }
}

