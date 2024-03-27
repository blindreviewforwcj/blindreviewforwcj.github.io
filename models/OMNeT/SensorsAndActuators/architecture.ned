

simple WaterLevel
{
    parameters:
        @display("i=sealevel");
        @icon(images/drone.png);

    gates:
        output data;
}

simple RainPrecipitation
{
    parameters:
        @display("i=rain");
        @icon(images/drone.png);

    gates:
        output data;
}


simple WaterVolume
{
    parameters:
        @display("i=watervolume");
        @icon(images/drone.png);

    gates:
        output data;
}

simple WaterFlow
{
    parameters:
        @display("i=drop");
        @icon(images/drone.png);

    gates:
        input data;
}


simple Southbound
{
    parameters:
        @display("i=device/card");
        @icon(images/drone.png);

    gates:
        input dataWaterLevel;
        input dataRainPrecipitation;
        input dataWaterVolume;
        output data;

        input decisiondataWaterFlow;

        output dataWaterFlow;

}

simple Fog
{
    @display("i=fog");
    gates:
        input dataToFog;
        output fogToData;
}


simple Exchange
{
    @display("i=exchange");
    gates:
        input data;
        output dataToFog;
        output decisiondataWaterFlow;
        input fogToData;
}

//
// Two instances (tic and toc) of Txc1 connected both ways.
// Tic and toc will pass messages to one another.
//
network architecture
{
    @display("bgb=1165.6124,848.25");
    submodules:
        WL: WaterLevel {
            @display("p=122.85,138.9375;is=n");
        }
        RP: RainPrecipitation {
            @display("p=122.85,365.625;is=n");
        }
        WV: WaterVolume {
            @display("p=136.0125,604.0125;is=n");
        }
        Southbound: Southbound {
            @display("p=400.72498,365.625");
        }
        Exchange: Exchange {
            @display("p=808.76245,365.625");
        }
        WF: WaterFlow {
            @display("p=400.72498,713.69995;is=n;i=,,30");
        }
        Fog: Fog {
            @display("p=1082.25,365.625");
        }
    connections:
        RP.data --> {  delay = 100ms; } --> Southbound.dataRainPrecipitation;
        WL.data --> {  delay = 100ms; } --> Southbound.dataWaterLevel;
        WV.data --> {  delay = 100ms; } --> Southbound.dataWaterVolume;
        Southbound.dataWaterFlow --> {  delay = 100ms; } --> WF.data;
        Southbound.data --> {  delay = 100ms; } --> Exchange.data;
        Exchange.dataToFog --> {  delay = 100ms; } --> Fog.dataToFog;
        Fog.fogToData --> {  delay = 100ms; } --> Exchange.fogToData;
        Exchange.decisiondataWaterFlow --> {  delay = 100ms; } --> Southbound.decisiondataWaterFlow;
}
