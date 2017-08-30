%% ==============main函数===============
clc
clear all;

%% 一些初始化
% 系统参数初始化
data_len = 1920;
codeRate = 1/2;          % 1/2, 3/4, 5/6(not supported)
info = round(rand(1, data_len));
interleave_pattern = interleave(data_len);
SNR = 3;
constel = exp(1j * 2 * pi / 8 * [1, 3, 7, 5]) ;
raw_ber = zeros(1, length(SNR));
coded_ber = zeros(1, length(SNR));
%% 产生原始数据
tx_bits = randi([0 1], [1, data_len]);
coded_bits = turbo_encode(tx_bits,codeRate,interleave_pattern);

%% 调制
temp_bits = bi2de(reshape(coded_bits, 2, []).', 2, 'left-msb').';
ytx_sym = constel(temp_bits + 1);

%% 加噪 & 接收处理
%for snr = 1 : length(SNR)
    % 加噪
    %noise_power = 10 ^ (- SNR(snr) / 10);
%     noise_power = 10 ^ (- 3 / 10);
%     noise = sqrt(noise_power / 2) * (randn(size(ytx_sym)) + 1j * randn(size(ytx_sym))); 
%     yrx_sym = ytx_sym + noise;
    yrx_sym = ytx_sym;

    % 接收处理
    LLR = zeros(size(coded_bits));
    LLR(1 : 2 : end) = -sqrt(2) * imag(yrx_sym);
    LLR(2 : 2 : end) = -sqrt(2) * real(yrx_sym);
    decoded_bits = zeros(1, length(coded_bits));
    decoded_bits(LLR > 0) = 1;
    
    input = LLR;
    %% 还原turbo码的输入
m = 3;
if codeRate == 1/2   
    len = length(input)/2 - 2*m;
    lenWithTail = length(input)/2;
    sys_info = input(1:2:end);
    check_info = input(2:2:end);   
    % rsc1的输入
    rsc_in1 = zeros(2,lenWithTail);
    rsc_in1(1,1:end) = sys_info(1:end);
    rsc_in1(2,1:2:end) = check_info(1:2:end);
    % rsc2的输入
    rsc_in2 = zeros(2,lenWithTail);
    rsc_in2(1, 1:len) = sys_info(1, interleave_pattern);
    rsc_in2(1, end-5:end) = sys_info(1,end-5:end);
    rsc_in2(2,2:2:end) = check_info(2:2:end);
end

if codeRate == 3/4   
    len = (length(input)-2*2*m)*3/4;     %去掉尾部12个比特
    lenWithTail = len + 2*m;
    sys_info = zeros(1,lenWithTail);
    check_info = zeros(1,lenWithTail);   
    cnt = 1;
    for n = 1: length(input)-2*2*m
        if mod(n,8)==2 || mod(n,8)==0
            check_info(cnt-1) = input(n);
        else
             sys_info(cnt) = input(n);           
             cnt = cnt+1;
        end
    end
    for n = length(input)-2*2*m+1 : length(input)
        if mod(n,2)==0
            check_info(cnt-1) = input(n);
        else          
            sys_info(cnt) = input(n);
            cnt = cnt+1;
        end
    end 
    % rsc1的输入
    rsc_in1 = zeros(2,lenWithTail);
    rsc_in1(1,1:end) = sys_info(1:end);
    for n = 1:1920
        if mod(n,6) == 1
            rsc_in1(2,n)= check_info(n);
        end
    end
    for n = 1921:1923
        rsc_in1(2,n)=check_info(n);
    end       
        
    % rsc2的输入
    rsc_in2 = zeros(2,lenWithTail);
    rsc_in2(1, 1:len) = sys_info(1, interleave_pattern);
    rsc_in2(1, end-5:end) = sys_info(1,end-5:end);
    for n = 1:1920
        if mod(n,6) == 0
            rsc_in2(2,n)= check_info(n);
        end
    end
    for n = 1924:1926
        rsc_in2(2,n)= check_info(n);
    end  
end

%% turbo译码
iteras = 1;
apri = zeros(1, len + 2*m);
for it = 1 : iteras
    % 第一个rsc的译码
    [tmp,apri] = max_log_map(rsc_in1, apri);
    apri(1:end-6) = apri(interleave_pattern);
    apri(end - 5 : end) = 0;
    % 第二个rsc的译码
    [soft_value2, apri] = max_log_map(rsc_in2, apri);
    apri(interleave_pattern) = apri(1:end-6);
    apri(end - 5 : end) = 0;
    
    %% 译码结束，输出
    soft_value2(interleave_pattern) = soft_value2(1 : len);
    output = (sign(soft_value2(1 : len)) + 1) / 2;
    coded_ber = sum(abs(output - tx_bits))
end
    
    
    %rx_bits = turbo_decode(LLR,codeRate,interleave_pattern, tx_bits);

    % 统计
    %raw_ber(snr) = raw_ber(snr) + sum(abs(decoded_bits(1 : 3072) - coded_bits(1 : 3072)));
    %coded_ber(snr) = coded_ber(snr) + sum(abs(rx_bits - tx_bits));
%end






