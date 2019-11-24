/* Units */

const pi = Math.PI;
const d = 1.0;
const cpd = 1.0;
const deg = Math.PI/180;
const rad = 1.0;
const grad = Math.PI/200;
const ms = 1.0;
const s = 1000*ms;
const um = 1e-3*ms;
const ns = 1e-6*ms;
const min = 60*s;
const hour = 60*min;
const Hz = 1.0;
const kHz = 1000.0*Hz;
const dps = 1.0;
const nA = 1.0;


let lgn_on = {
    type: "layer",
    stimulus_acceptable: true,
    output: {
        resolution_x: 0.05,
        resolution_y: 0.05
    },
    mechanism: "abstract:glm.lgn",
    saturation: {
        type: "processor",
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
    }
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

let brain_lgn_to_v1_connection_trial = {
    type: "brain",
    networks: {
        lgn: lgn_directional
    }
}


let simulation_job = {
    type: "job",
    mechanism: "job:simulate",
    output_folder: "default_folder"
};

gabor_grating_stimulus = {
    type: "processor",
    mechanism: "stimulus:stationary.gabor-grating",
    grid_x: 111,
    grid_y: 111,
    size_x: 2.2*d,
    size_y: 2.2*d,
    luminance: 0.5,
    contrast: 1.0,
    spatial_frequency: 0.83*cpd,
    spatial_phase: 0,
    orientation: 45*deg,
    prestimulus_epoch: 50*ms,
    stimulus_duration: 400*ms,
    poststimulus_epoch: 400*ms
};

moving_gabor_grating_stimulus = {
    type: "processor",
    mechanism: "stimulus:moving.gabor-grating",
    grid_x: 51,
    grid_y: 51,
    size_x: 1.0*d,
    size_y: 1.0*d,
    luminance: 0.5,
    contrast: 1.0,
    spatial_frequency: 1.0*cpd,
    temporal_frequency: 10.0*Hz,
    spatial_phase: 0,
    orientation: 45*deg,
    prestimulus_epoch: 400*ms,
    stimulus_duration: 400*ms,
    poststimulus_epoch: 400*ms
};

rectangular_grating_stimulus = {
    type: "processor",
    mechanism: "stimulus:stationary.rectangular-grating",
    grid_x: 51,
    grid_y: 51,
    size_x: 1.0*d,
    size_y: 1.0*d,
    luminance: 0.5,
    contrast: 1.0,
    spatial_frequency: 3.0*cpd,
    spatial_phase: 0,
    orientation: 45*deg,
    prestimulus_epoch: 400*ms,
    stimulus_duration: 400*ms,
    poststimulus_epoch: 400*ms
};

moving_rect_grating = {
    type: "processor",
    mechanism: "stimulus:moving.rectangular-grating",
    grid_x: 51,
    grid_y: 51,
    size_x: 1.0*d,
    size_y: 1.0*d,
    luminance: 0.5,
    contrast: 1.0,
    spatial_frequency: 3.0*cpd,
    temporal_frequency: 10*Hz,
    spatial_phase: 0,
    orientation: 45*deg,
    prestimulus_epoch: 400*ms,
    stimulus_duration: 400*ms,
    poststimulus_epoch: 400*ms
};

stationary_dot_stimulus = {
    type: "processor",
    mechanism: "stimulus:stationary.dot",
    grid_x: 51,
    grid_y: 51,
    size_x: 1.0*d,
    size_y: 1.0*d,
    luminance: 0.0,
    contrast: 1.0,
    prestimulus_epoch: 400*ms,
    stimulus_duration: 400*ms,
    poststimulus_epoch: 400*ms,
    x: -0.25*d,
    y: -0.4*d,
    radius: 0.10*d
};

moving_dot_stimulus = {
    type: "processor",
    mechanism: "stimulus:moving.dot",
    grid_x: 51,
    grid_y: 51,
    size_x: 1.0*d,
    size_y: 1.0*d,
    luminance: 0.0,
    contrast: 1.0,
    prestimulus_epoch: 400*ms,
    stimulus_duration: 400*ms,
    poststimulus_epoch: 400*ms,
    x: 0.0*d,
    y: 0.0*d,
    radius: 0.1*d,
    velocity_x: 3*dps,
    velocity_y: 3*dps
};

stationary_bar_stimulus = {
    type: "processor",
    mechanism: "stimulus:stationary.bar",
    grid_x: 51,
    grid_y: 51,
    size_x: 1.0*d,
    size_y: 1.0*d,
    luminance: 0.0,
    contrast: 1.0,
    prestimulus_epoch: 400*ms,
    stimulus_duration: 400*ms,
    poststimulus_epoch: 400*ms,
    length: 0.3*d,
    width: 0.05*d,
    x: 0.0*d,
    y: 0.0*d,
    orientation: 90*deg
};

moving_bar_stimulus = {
    type: "processor",
    mechanism: "stimulus:moving.bar",
    grid_x: 51,
    grid_y: 51,
    size_x: 1.0*d,
    size_y: 1.0*d,
    luminance: 0.0,
    contrast: 1.0,
    prestimulus_epoch: 400*ms,
    stimulus_duration: 400*ms,
    poststimulus_epoch: 400*ms,
    length: 0.3*d,
    width: 0.05*d,
    x: 0.0*d,
    y: 0.0*d,
    orientation: 45*deg,
    speed: 3.0*dps
};

stationary_stimulus_reader = {
    type: "processor",
    mechanism: "stimulus:stationary.reader",
    grid_x: 51,
    grid_y: 51,
    size_x: 1.0*d,
    size_y: 1.0*d,
    luminance: 0.0,
    contrast: 1.0,
    prestimulus_epoch: 400*ms,
    stimulus_duration: 400*ms,
    poststimulus_epoch: 400*ms,
    filename: "sample-stimulus.bin"
};

stimulus_stream = {
    type: "processor",
    mechanism: "stimulus:moving.stream",
    grid_x: 51,
    grid_y: 51,
    size_x: 1.0*d,
    size_y: 1.0*d,
    luminance: 0.5,
    contrast: 1.0,
    prestimulus_epoch: 400*ms,
    stimulus_duration: 400*ms,
    poststimulus_epoch: 400*ms,
    filename: "sample-stimulus.bin"
};

test_stimulus = {
    type: "processor",
    mechanism: "stimulus:external.test",
    grid_x: 51,
    grid_y: 51,
    size_x: 1.0*d,
    size_y: 1.0*d,
    luminance: 0.5,
    contrast: 1.0,
    prestimulus_epoch: 400*ms,
    stimulus_duration: 400*ms,
    poststimulus_epoch: 400*ms,
    sigma_speed: 1.5
};

top_right_dot = Object.create(stationary_dot_stimulus);
with (top_right_dot){
    x = 0.25*d;
    y = 0.25*d;
}

top_left_dot = Object.create(stationary_dot_stimulus);
with (top_left_dot){
    x = -0.25*d;
    y = 0.25*d;
}

bottom_right_dot = Object.create(stationary_dot_stimulus);
with (bottom_right_dot){
    x = 0.25*d;
    y = -0.25*d;
}

bottom_left_dot = Object.create(stationary_dot_stimulus);
with (bottom_left_dot){
    x = -0.25*d;
    y = -0.25*d;
}

test_weighted_stimulus = {
    type: "processor",
    mechanism: "stimulus:complex.weighted",
    grid_x: 51,
    grid_y: 51,
    size_x: 1.0*d,
    size_y: 1.0*d,
    luminance: 0.5,
    contrast: 1.0,
    content: [top_right_dot, top_left_dot, bottom_right_dot, bottom_left_dot],
    weights: [1, 1, 1, 1]
};

dot_mask = Object.create(stationary_dot_stimulus);
with (dot_mask) {
    x = 0.0;
    y = 0.0;
    radius = 0.3*d;
}

test_bounded_stimulus = {
    type: "processor",
    mechanism: "stimulus:complex.bounded",
    grid_x: 51,
    grid_y: 51,
    size_x: 1.0*d,
    size_y: 1.0*d,
    luminance: 0.5,
    contrast: 1.0, // useless
    content: [dot_mask, gabor_grating_stimulus]
};

test_sequence_stimulus = {
    type: "processor",
    mechanism: "stimulus:complex.sequence",
    grid_x: 51,
    grid_y: 51,
    size_x: 1.0*d,
    size_y: 1.0*d,
    luminance: 0.5,
    contrast: 1.0,
    shuffle: true,
    repeats: 3,
    name: "my_sequence",
    content: []
};

[0, 30.0*deg, 60.0*deg, 90.0*deg, 120.0*deg, 160.0*deg].forEach(function(orientation){
    var stimulus = Object.create(gabor_grating_stimulus);
    stimulus.orientation = orientation;
    test_sequence_stimulus.content.push(stimulus);
});

let stimulus_list = {
    stationary_gabor_grating: gabor_grating_stimulus,
    stationary_rectangular_grating: rectangular_grating_stimulus,
    stationary_dot_stimulus: stationary_dot_stimulus,
    stationary_bar_stimulus: stationary_bar_stimulus,
    stationary_stimulus_reader: stationary_stimulus_reader,
    moving_gabor_grating: moving_gabor_grating_stimulus,
    moving_rectangular_grating: moving_rect_grating,
    moving_bar: moving_bar_stimulus,
    moving_dot: moving_dot_stimulus,
    moving_stream: stimulus_stream,
    complex_weighted: test_weighted_stimulus,
    complex_bounded: test_bounded_stimulus,
    complex_sequence: test_sequence_stimulus
};


no_saturation = {
    type: "processor",
    mechanism: "glm:stimulus_saturation.no",
    dark_current: 0,
    amplification: 40.0*nA
};

broken_line_saturation = {
    type: "processor",
    mechanism: "glm:stimulus_saturation.broken_line",
    dark_current: 0,
    amplification: 40.0*nA,
    max_current: 20.0*nA
};

half_sigmoid_saturation = {
    type: "processor",
    mechanism: "glm:stimulus_saturation.half_sigmoid",
    dark_current: 0,
    amplification: 40.0*nA,
    max_current: 20.0*nA
};

let saturation_list = {
    no: no_saturation,
    broken_line: broken_line_saturation,
    half_sigmoid: half_sigmoid_saturation
};

let temporal_kernel_list = {
    ode: {
        type: "processor",
        mechanism: "glm:temporal_kernel.ode",
        tau: 10*ms,
        tau_late: 60*ms,
        initial_stimulus_value: 20.0,
        K: 1
    }
};

let spatial_kernel_list = {
    gaussian: {
        type: "processor",
        mechanism: "glm:spatial_kernel.gaussian",
        radius: 0.3*d
    }
};

temporal_kernel_excitatory = Object.create(temporal_kernel_list.ode);
temporal_kernel_inhibitory = Object.create(temporal_kernel_list.ode);
temporal_kernel_inhibitory.tau = 20*ms;

spatial_kernel_excitatory = Object.create(spatial_kernel_list.gaussian);
spatial_kernel_inhibitory = Object.create(spatial_kernel_list.gaussian);
spatial_kernel_inhibitory.radius = 0.6*d;

let dog_filter = {
    type: "processor",
    mechanism: "glm:dog",
    dark_rate: 10.0*Hz,
    surround_weight: 1.0,
    threshold: 0
};



let world = {

    application: {
        type: "application",
        parent: "mpirun",
        configuration_mode: "simple",
        process_number: 4,
        output_folder_prefix: "test"
    },

    stimulus: stimulus_list.stationary_gabor_grating,

    test_field: saturation_list.no,
    test_field_2: temporal_kernel_excitatory,
    test_field_3: spatial_kernel_excitatory,
    test_field_i2: temporal_kernel_inhibitory,
    test_field_i3: spatial_kernel_inhibitory,
    test_field_4: dog_filter,

    brain: brain_lgn_to_v1_connection_trial,

    analysis: {

    },

    job: simulation_job

};