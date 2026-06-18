function [adc_P, adc_T] = bmp280_sensor(P_true, T_true_K, OSR_T, OSR_P, IIR_coeff)
% =========================================================
% BMP280 Sensor Hardware Model
% Outputs raw 20-bit ADC values (what the MCU reads via I2C)
% Includes: ADC quantization, noise, oversampling, IIR filter
% =========================================================

T_true_C = T_true_K - 273.15;

% --- Calibration coefficients ---
dig_T1 = 27504;  dig_T2 = 26435;  dig_T3 = -1000;
dig_P1 = 36477;  dig_P2 = -10685; dig_P3 = 3024;
dig_P4 = 2855;   dig_P5 = 140;    dig_P6 = -7;
dig_P7 = 15500;  dig_P8 = -14600; dig_P9 = 6000;

% --- Invert temperature compensation → ideal adc_T ---
T_fine_true = T_true_C * 5120.0;
disc_T = max(64*double(dig_T2)^2 + 4*double(dig_T3)*T_fine_true, 0);
u = (-8*double(dig_T2) + sqrt(disc_T)) / (2*double(dig_T3));
adc_T_ideal = (u + double(dig_T1)/8192.0) * 131072.0;

% --- ADC noise + oversampling (temperature) ---
adc_T = round(adc_T_ideal + (8.0/sqrt(double(OSR_T))) * randn());

% --- Invert pressure compensation → ideal adc_P ---
% Need t_fine from ideal adc_T first
var1_T = (adc_T_ideal/16384.0 - double(dig_T1)/1024.0) * double(dig_T2);
var2_T = (adc_T_ideal/131072.0 - double(dig_T1)/8192.0)^2 * double(dig_T3);
t_fine_ideal = var1_T + var2_T;

A = t_fine_ideal/2.0 - 64000.0;
B = (A^2*double(dig_P6)/32768.0 + A*double(dig_P5)*2.0)/4.0 + double(dig_P4)*65536.0;
C = (1.0 + (double(dig_P3)*A^2/524288.0 + double(dig_P2)*A)/524288.0/32768.0)*double(dig_P1);

aq = double(dig_P9)/2147483648.0;
bq = 16.0 + double(dig_P8)/32768.0;
cq = double(dig_P7) - 16.0*P_true;
p_int = (-bq + sqrt(max(bq^2 - 4*aq*cq, 0))) / (2*aq);
adc_P_ideal = 1048576.0 - B/4096.0 - p_int*C/6250.0;

% --- ADC noise + oversampling (pressure) ---
adc_P = round(adc_P_ideal + (12.0/sqrt(double(OSR_P))) * randn());

% --- On-chip IIR filter (runs inside BMP280, before I2C output) ---
persistent adc_P_filt adc_T_filt;
if isempty(adc_P_filt)
    adc_P_filt = adc_P;
    adc_T_filt = adc_T;
end

if IIR_coeff > 1
    adc_P = round(adc_P_filt*(1.0 - 1.0/double(IIR_coeff)) + double(adc_P)/double(IIR_coeff));
    adc_T = round(adc_T_filt*(1.0 - 1.0/double(IIR_coeff)) + double(adc_T)/double(IIR_coeff));
end

adc_P_filt = adc_P;
adc_T_filt = adc_T;

end