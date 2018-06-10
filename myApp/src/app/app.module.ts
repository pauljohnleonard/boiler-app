import { NgModule, ErrorHandler } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { IonicApp, IonicModule, IonicErrorHandler } from 'ionic-angular';
import { MyApp } from './app.component';

import { AboutPage } from '../pages/about/about';
import { SetupPage } from '../pages/setup/setup';
import { HomePage } from '../pages/home/home';
import { TabsPage } from '../pages/tabs/tabs';

import { StatusBar } from '@ionic-native/status-bar';
import { SplashScreen } from '@ionic-native/splash-screen';
import { HttpClientModule } from '@angular/common/http';
import { NetworkInterface } from '@ionic-native/network-interface';
import { ConfigProvider } from '../providers/config/config';
import { IpSetupProvider } from '../providers/ipsetup/ipsetup';
import { ModelProvider } from '../providers/model/model';

@NgModule({
  declarations: [
    MyApp,
    AboutPage,
    SetupPage,
    HomePage,
    TabsPage
  ],
  imports: [
    BrowserModule,
    HttpClientModule,
    IonicModule.forRoot(MyApp)
  ],
  bootstrap: [IonicApp],
  entryComponents: [
    MyApp,
    AboutPage,
    SetupPage,
    HomePage,
    TabsPage
  ],
  providers: [
    StatusBar,
    IpSetupProvider,
    SplashScreen,
    NetworkInterface,
    ModelProvider,
    {provide: ErrorHandler, useClass: IonicErrorHandler},
    ConfigProvider,
    IpSetupProvider,
    ModelProvider
  ]
})
export class AppModule {}
