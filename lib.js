let lgn_on = {
    type: "layer",
    stimulus_acceptable: true,
    output: {
        resolution_x: 0.05,
        resolution_y: 0.05
    },
    mechanism: "abstract:glm.lgn",
    saturation: {
        type: "abstract_saturatopn",
        enabled: true,
        mechanism: "abstract_saturation:half_of_sigmoid",
        saturation_scale: 0.0
    },
    temporal_filter: {
        tau_center: 10,
        tau_surround: 20,
        tau_late: 64,
        K: 1,
        saturation: 100
    },
    spatial_filter: {
        sigma_center: 0.30,
        sigma_surround: 1.50,
        K: 1,
        E0: 0
    },
    L_spont: 10,
    latency: 0,
    rf_type: "on",
    latency: 20
};

let lgn_off = Object.create(lgn_on);
lgn_off.rf_type = "off";

let lgn_on_lagged = Object.create(lgn_on);
let lgn_off_lagged = Object.create(lgn_off);
lgn_on_lagged.latency = lgn_off_lagged.latency = 80;



let lgn = {
    type: "network",
    networks: {
        on: lgn_on,
        off: lgn_off
    }
};

let lgn_directional = {
    type: "network",
    output: {
        width: 6,
        height: 6
    },
    networks: {
        lagged_on: lgn_on_lagged,
        lagged_off: lgn_off_lagged,
        non_lagged_on: lgn_on,
        non_lagged_off: lgn_off
    },
    input: [],
    output: ["lagged_on", "lagged_off", "non_lagged_on", "non_lagged_off"]
};

let cbrd_layer = {
    type: "layer",
    mechanism: "cbrd"
    // the rest parameters will be given further
}

let cbrd_exc_layer = Object.create(cbrd_layer);

let cbrd_inh_layer = Object.create(cbrd_layer);

let conn_standard_200 = {
    mechanism: "connections:artificial.standard",
    sigma: 200
}


let std_cbrd = {
    networks: {
        exc: cbrd_exc_layer,
        inh: cbrd_inh_layer
    },
    input: ["exc", "inh"],
    output: ["exc", "inh"],
    connections: [{
        source: "exc",
        target: "inh",
        weight: 1.0,
        profile: conn_standard_200
    }, {
        source: "inh",
        target: "exc",
        weight: 1.0,
        profile: conn_standard_200
    }]
};

let std_l4_cbrd = Object.create(std_cbrd);

let std_l23_cbrd = Object.create(std_cbrd);



let v1_cbrd = {
    parameters: {
        map: {
            purpose: "directional",
            mechanism: "map:artificial.symmetric",
            pinwheels: 4
        }
    },
    output: {
        width: 1400,
        height: 900,
    },
    networks: {
        l4: std_l4_cbrd,
        l23: std_l23_cbrd
    },
    input: ["l4.exc", "l4.inh"],
    output: ["l23.exc"],
    connections: [
        {
            source: "l4",
            target: "l23",
            weight: 1.0,
            profile: conn_standard_200
        }
    ]
};


let simulation_job = {
    type: "job",
    mechanism: "job:simulate",
    output_folder: "default_folder"
};


let world = {

    application: {
        type: "application",
        parent: "mpirun",
        configuration_mode: "simple",
        process_number: 4,

        log: {

        }
    },

    brain: {
        type: "brain",
        networks: {
            lgn: lgn_directional
        }
    },

    stimulus: {

    },


    analysis: {

    },


    job: simulation_job

};