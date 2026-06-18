clear all;

% --- BMP Configuration ---
OSR_T     = 2;   % Temperature oversampling (1,2,4,8,16)
OSR_P     = 16;  % Pressure oversampling    (1,2,4,8,16)
IIR_coeff = 4;   % IIR filter coefficient   (0=off, 2,4,8,16)

t_measure_ms = 1 + 2.3*OSR_T + 2.3*OSR_P + 0.575;

model = 'sensors_simulation';
load_system(model);

set_param(model, 'StopTime',     '35'); 
set_param(model, 'EnablePacing', 'on');  
set_param(model, 'PacingRate',   1);     

sim(model);