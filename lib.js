/**
 * A sample model for the abstract layer
 *
 * @param type "layer" for the layer (have mechanisms instead of subnetworks)
 * @param type "network" for the network (have subnetworks instead of mechanisms)
 * @param stimuulus_acceptable true is the layer accepts the stimulus input, false otherwise
 * @param output properties of the output data: matrix width and height and resolution on X and Y
 */
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
    },
    input: [],
    output: ["on", "off"]
}


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
}




/**
 * General properties of the stimulation process
 *
 * @param application the general properties of the application
 * @param brain properties of the brain
 * @param stimulus the stimulus properties
 * @param analysis the methods for the analysis
 * @param trainer (optional but required when application.reason == "training") the trainer
 */
let world = {

    /**
     * Defines the general parameters of the simulation process
     *
     * @param parent name of the parent application ("mpirun" for mpich)
     * @param configuration_mode 'simple' for running on the standalone computers, 'complex' for running on clusters
     * @param process_number total number of all processes
     * @param log the object responsible for the logging process
     * @param reason the general reason for the simulation 'simulate' - run stand-alone simulation and save the results
     * into the hard disk drive
     */
    application: {
        type: "application",
        parent: "mpirun",
        configuration_mode: "simple",
        process_number: 4,
        reason: "simulate",

        /**
         * Parameters of the logging process
         */
        log: {

        }
    },

    /**
     * Properties of the network to simulate
     *
     * @param parameters (optional, but required for some types of the brain) global parameters of the brain
     * @param networks all sub-networks containing in the netwoek
     * @param connections represents the connections betwween the children
     */
    brain: {
        type: "brain",
        networks: {
            lgn: lgn_directional/*,
            v1: v1_cbrd */
        }/*,
        connections: {
            source: "lgn",
            target: "v1",
            weight: 10.0,
            profile: {
                mechanism: "connections:artificial.hubel-wiesel",
                sigma_x: 200,
                sigma_y: 20,
                magnification_factor: 1.5
            }
        } */
    },

    /**
     * Stimulation parameters
     */
    stimulus: {

    },

    /**
     * The analyzer parameters
     */
    analysis: {

    },

    /**
     * Information about the purpose of the simulation tasl
     */
    job: simulation_job



};