function [soft_value, extern_value] = max_log_map(input, apri)

%% ===========状态转移图开始===========
... S(i)  0-S(i+1)/b(i)b(i+1)  1-S(i+1)/b(i)b(i+1)
... 1           1/00                5/11 
... 2           5/00                1/11
... 3           6/01                2/10
... 4           2/01                6/10
... 5           3/01                7/10
... 6           7/01                3/10
... 7           8/00                4/11
... 8           4/00                8/11
... ===========状态转移图结束===========

%% 初始化
len = length(input);
infty = 128;
state = 2 ^ 3;
soft_value = zeros(1, len);
next_state = [1 5 6 2 3 7 8 4;...
              5 1 2 6 7 3 4 8];
last_state = [1 4 5 8 2 3 6 7;...
              2 3 6 7 1 4 5 8];
% g = [1 1 0 1;
%      1 1 1 1];
% [tmp1, next_state, tmp2, last_state] = trellis(g);
%初始化软信息
x = input(1, :);
y = input(2, :);
%初始化alpha、beta和gamma
alpha = - infty * ones(state, len);
alpha(1, 1) = 0;
beta = - infty * ones(state, len + 1);
beta(1, len + 1) = 0;
gamma = zeros(state, 2, len);
gamma([3, 4, 5, 6], 1, :) = repmat(y, 4, 1);
gamma([3, 4, 5, 6], 2, :) = repmat(x + apri, 4, 1);
gamma([1, 2, 7, 8], 2, :) = repmat(x + y + apri, 4, 1);

%% 计算分支度量alpha
for k = 2 : len
    alpha(:, k) = max(squeeze(gamma(last_state(1, :), 1, k - 1)) + alpha(last_state(1, :), k - 1), ...
                      squeeze(gamma(last_state(2, :), 2, k - 1)) + alpha(last_state(2, :), k - 1));
end

%% 计算分支度量beta和LLR
for k = len : -1 : 1
    beta(:, k) = max(squeeze(gamma(:, 1, k)) + beta(next_state(1, :), k + 1), ...
                     squeeze(gamma(:, 2, k)) + beta(next_state(2, :), k + 1));
    soft_value(k) = max(alpha(:, k) + squeeze(gamma(:, 2, k)) + beta(next_state(2, :), k + 1)) - ...
                    max(alpha(:, k) + squeeze(gamma(:, 1, k)) + beta(next_state(1, :), k + 1)) ;
end

extern_value = soft_value - x - apri;
