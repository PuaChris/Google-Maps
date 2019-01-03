#include <random>
#include <unittest++/UnitTest++.h>

#include "StreetsDatabaseAPI.h"
#include "m1.h"
#include "m3.h"

#include "unit_test_util.h"
#include "path_verify.h"

using ece297test::relative_error;
using ece297test::path_is_legal;


SUITE(inter_inter_path_perf_basic_public) {
struct MapFixture {
    MapFixture() {
        rng = std::minstd_rand(4);
        rand_intersection = std::uniform_int_distribution<unsigned>(0, getNumberOfIntersections()-1);
        rand_street = std::uniform_int_distribution<unsigned>(1, getNumberOfStreets()-1);
        rand_segment = std::uniform_int_distribution<unsigned>(0, getNumberOfStreetSegments()-1);
        rand_poi = std::uniform_int_distribution<unsigned>(0, getNumberOfPointsOfInterest()-1);
        rand_lat = std::uniform_real_distribution<double>(43.479999542, 43.919979095);
        rand_lon = std::uniform_real_distribution<double>(-79.789978027, -79.000007629);
        rand_turn_penalty = std::uniform_real_distribution<double>(0., 30.);
    }

    std::minstd_rand rng;
    std::uniform_int_distribution<unsigned> rand_intersection;
    std::uniform_int_distribution<unsigned> rand_street;
    std::uniform_int_distribution<unsigned> rand_segment;
    std::uniform_int_distribution<unsigned> rand_poi;
    std::uniform_real_distribution<double> rand_lat;
    std::uniform_real_distribution<double> rand_lon;
    std::uniform_real_distribution<double> rand_turn_penalty;
};
    TEST_FIXTURE(MapFixture, find_path_to_point_of_interest_perf_basic) {
        //Verify Functionality
        std::vector<unsigned> path;
        std::vector<unsigned> valid_end_intersections;
        double path_cost;
        
        valid_end_intersections = {14599};
        path = find_path_to_point_of_interest(14893, "St. Lawrence Dental", 15.00000000000000000);
        path_cost = compute_path_travel_time(path, 15.00000000000000000);
        CHECK(path_is_legal(14893, valid_end_intersections, path));
        CHECK(path_cost <= 223.85647327515300731);
        
        valid_end_intersections = {3380, 1375, 52868, 12183, 9737, 21562, 96170, 2600, 8874, 2318, 60770, 98646, 18142, 22183, 71187};
        path = find_path_to_point_of_interest(18932, "Toronto Sun", 15.00000000000000000);
        path_cost = compute_path_travel_time(path, 15.00000000000000000);
        CHECK(path_is_legal(18932, valid_end_intersections, path));
        CHECK(path_cost <= 366.49948943156311998);
        
        valid_end_intersections = {28326};
        path = find_path_to_point_of_interest(22755, "Bramalea City Centre Bus Terminal", 15.00000000000000000);
        path_cost = compute_path_travel_time(path, 15.00000000000000000);
        CHECK(path_is_legal(22755, valid_end_intersections, path));
        CHECK(path_cost <= 2434.89613255445146933);
        
        valid_end_intersections = {99803};
        path = find_path_to_point_of_interest(24946, "Shopper's Drug Mart", 15.00000000000000000);
        path_cost = compute_path_travel_time(path, 15.00000000000000000);
        CHECK(path_is_legal(24946, valid_end_intersections, path));
        CHECK(path_cost <= 2574.91923574898237348);
        
        valid_end_intersections = {20087, 14051};
        path = find_path_to_point_of_interest(27711, "South Street Burger", 15.00000000000000000);
        path_cost = compute_path_travel_time(path, 15.00000000000000000);
        CHECK(path_is_legal(27711, valid_end_intersections, path));
        CHECK(path_cost <= 1645.87128052991738514);
        
        valid_end_intersections = {56379, 64261, 51410, 4843, 99333, 29736};
        path = find_path_to_point_of_interest(28976, "Wimpy's Diner", 15.00000000000000000);
        path_cost = compute_path_travel_time(path, 15.00000000000000000);
        CHECK(path_is_legal(28976, valid_end_intersections, path));
        CHECK(path_cost <= 1077.06330502329910814);
        
        valid_end_intersections = {24760};
        path = find_path_to_point_of_interest(29884, "China China Palace", 15.00000000000000000);
        path_cost = compute_path_travel_time(path, 15.00000000000000000);
        CHECK(path_is_legal(29884, valid_end_intersections, path));
        CHECK(path_cost <= 1796.64133172528067917);
        
        valid_end_intersections = {3972};
        path = find_path_to_point_of_interest(37103, "Home of Hot Taste", 15.00000000000000000);
        path_cost = compute_path_travel_time(path, 15.00000000000000000);
        CHECK(path_is_legal(37103, valid_end_intersections, path));
        CHECK(path_cost <= 2124.37870531606358782);
        
        valid_end_intersections = {99983, 98776, 44741, 88, 100647, 44637};
        path = find_path_to_point_of_interest(39889, "aroma espresso bar", 15.00000000000000000);
        path_cost = compute_path_travel_time(path, 15.00000000000000000);
        CHECK(path_is_legal(39889, valid_end_intersections, path));
        CHECK(path_cost <= 1549.66794630845697611);
        
        valid_end_intersections = {9740, 2294};
        path = find_path_to_point_of_interest(48777, "Golden Griddle Family Restaurant", 15.00000000000000000);
        path_cost = compute_path_travel_time(path, 15.00000000000000000);
        CHECK(path_is_legal(48777, valid_end_intersections, path));
        CHECK(path_cost <= 1773.56266361129132747);
        
        valid_end_intersections = {12913, 3933, 18627, 18798, 23133, 19081, 54335, 48990, 53540, 35824, 49794, 59487, 69668, 72384, 8994, 3705, 28896, 4320, 19156, 4197, 81984, 100275, 106566, 56389, 52373, 53502, 10070, 15646, 71793, 71027, 71564, 62070, 54633, 5198, 77731, 52965, 77399, 2506, 5184, 46775, 79525, 33651, 26561, 94157, 58670, 105755, 90150, 96138, 23333, 16513, 34424, 27882, 92790, 96346, 8596, 3172};
        path = find_path_to_point_of_interest(49555, "Mr. Sub", 15.00000000000000000);
        path_cost = compute_path_travel_time(path, 15.00000000000000000);
        CHECK(path_is_legal(49555, valid_end_intersections, path));
        CHECK(path_cost <= 391.40771317837175047);
        
        valid_end_intersections = {4111};
        path = find_path_to_point_of_interest(48760, "Hoju Bistro Japanese & Korean Cuisine", 15.00000000000000000);
        path_cost = compute_path_travel_time(path, 15.00000000000000000);
        CHECK(path_is_legal(48760, valid_end_intersections, path));
        CHECK(path_cost <= 2338.72106989046824310);
        
        valid_end_intersections = {241, 59292, 18797, 102703, 105955, 18480, 88403, 12790, 71555, 24961, 66412, 16774, 8586, 52498, 15040, 95457, 88451, 18402, 76168, 85695, 79879, 9937, 67837, 41902, 27093, 82738, 39706, 36957, 33435, 31340, 1651, 60527, 90216, 83793, 1587, 48861, 25585, 94618, 78585};
        path = find_path_to_point_of_interest(56454, "Swiss Chalet", 15.00000000000000000);
        path_cost = compute_path_travel_time(path, 15.00000000000000000);
        CHECK(path_is_legal(56454, valid_end_intersections, path));
        CHECK(path_cost <= 80.62052502486626793);
        
        valid_end_intersections = {11356, 97837, 11261, 11880, 10506, 16695, 23605, 3519, 2722, 8336, 72180, 79760, 17355, 19206, 2650, 82444, 8129, 2142, 34930, 3380, 71514, 20566, 1220, 72384, 19842, 728, 59293, 88317, 11676, 59203, 30485, 53982, 55857, 75059, 4318, 4642, 90, 1182, 71356, 57953, 104553, 4820, 54499, 86612, 77674, 79038, 73908, 5015, 50343, 8675, 78714, 61127, 92170, 22166, 74778, 97103, 74655, 100349, 88337, 92839, 63416, 96491, 77823, 103985};
        path = find_path_to_point_of_interest(49549, "Coffee Time", 15.00000000000000000);
        path_cost = compute_path_travel_time(path, 15.00000000000000000);
        CHECK(path_is_legal(49549, valid_end_intersections, path));
        CHECK(path_cost <= 264.52094637212047701);
        
        valid_end_intersections = {2685, 99066, 33018, 2125, 16770, 16580, 1051, 1015, 4111, 14599, 89257, 44066, 99102, 103428, 8158, 12620, 1082, 26238, 23841, 1656, 4188, 4016, 49619, 82104, 3705, 93483, 36643, 36644, 79574, 28493, 60849, 71749, 38951, 14229, 16657, 2394, 26674, 6859, 8876, 52381, 2241, 70448, 9388, 14595, 87691, 79156, 28633, 29789, 12103, 2273, 4017, 1884, 96007, 9937, 14122, 19348, 62042, 50677, 16765, 2248, 2363, 91936, 10040, 87695, 98161, 41909, 34191, 92165, 32507, 90613, 104991, 48990, 104555, 97511, 60705, 4016, 74560, 82652, 7711, 32553, 94484, 73509, 1057, 3950, 63294, 93128, 73512, 105736, 1813, 4182, 94618, 62043, 91452, 82569, 43067};
        path = find_path_to_point_of_interest(66586, "Second Cup", 15.00000000000000000);
        path_cost = compute_path_travel_time(path, 15.00000000000000000);
        CHECK(path_is_legal(66586, valid_end_intersections, path));
        CHECK(path_cost <= 670.74713529707855741);
        
        valid_end_intersections = {1744, 41885, 4016, 85504, 2262, 86938};
        path = find_path_to_point_of_interest(51878, "Pumpernickel's", 15.00000000000000000);
        path_cost = compute_path_travel_time(path, 15.00000000000000000);
        CHECK(path_is_legal(51878, valid_end_intersections, path));
        CHECK(path_cost <= 919.49506753919843050);
        
        valid_end_intersections = {102025, 6339, 1606, 12751, 2376, 2324, 2290, 20025, 69682, 14820, 29104, 86232, 7765, 16975, 1014, 9244, 21369, 103429, 24460, 16462, 85315, 29210, 2954, 98707, 101446, 99962, 31614, 57675, 17010, 92414, 12581, 71793, 49815, 77452, 91245, 92502, 61261, 49579, 22702, 33213, 35826, 61521, 42735, 36000, 59406, 103939, 35151, 54004, 19844, 29581, 16757, 33014, 70058, 41887, 1451, 44754, 24760, 31089, 31857, 106573, 101979, 107521, 68572, 4309, 103323, 103248, 98622, 62004, 23686, 7225, 60370, 90042, 92175, 61318, 6724, 20730, 98946, 11442, 107221, 62041, 12338, 79040, 71545, 70909, 68931, 72646, 41218, 75682, 76109, 90534, 56109, 73344, 75072, 81689, 50236, 100978, 101900, 70747, 8233, 2272, 74435, 24813, 82898, 83204, 96009, 67794, 1387, 6056, 2242, 23123, 10854, 87690, 90, 66053, 66552, 36589, 81814, 90600, 57025, 33577, 32573, 24733, 3803, 98153, 99122, 105007, 725, 84727, 75746, 34810, 90766, 22148, 22525, 36643, 55855, 21221, 99329, 71569, 63993, 21715, 61723, 4650, 25731, 45402, 56308, 87078, 78708, 41490, 68958, 102029, 61137, 6668, 104741, 59974, 63316, 93394, 27002, 80410, 13495, 59482, 103983, 35794, 105725};
        path = find_path_to_point_of_interest(72912, "Shoppers Drug Mart", 15.00000000000000000);
        path_cost = compute_path_travel_time(path, 15.00000000000000000);
        CHECK(path_is_legal(72912, valid_end_intersections, path));
        CHECK(path_cost <= 185.16308461608377911);
        
        valid_end_intersections = {1080};
        path = find_path_to_point_of_interest(77844, "Sid's Southside Cafe", 15.00000000000000000);
        path_cost = compute_path_travel_time(path, 15.00000000000000000);
        CHECK(path_is_legal(77844, valid_end_intersections, path));
        CHECK(path_cost <= 1468.35806426563999594);
        
        valid_end_intersections = {3128, 1127, 87701, 1130, 9511, 7276, 7245, 8227, 91425, 16773, 16695, 20024, 20228, 1750, 16570, 71078, 8374, 88345, 1118, 97185, 103427, 18672, 59292, 99983, 24740, 18487, 98904, 41909, 9163, 28915, 57095, 80708, 88674, 107653, 61242, 61372, 3305, 105178, 105429, 77832, 43589, 36000, 36298, 54326, 61296, 97143, 104617, 16757, 2600, 44754, 28907, 19182, 5510, 1671, 60902, 70584, 26925, 68574, 4308, 82642, 29789, 44161, 7854, 91776, 86833, 61226, 71731, 54006, 90874, 18970, 1078, 23758, 108379, 3679, 64275, 4461, 88344, 70573, 1744, 20826, 14901, 12572, 14675, 333, 5702, 44066, 71262, 98900, 4318, 24452, 99720, 71027, 57995, 5812, 13135, 749, 61308, 71869, 54499, 70708, 2246, 1862, 6753, 2294, 30964, 44035, 52383, 69685, 9799, 8172, 98946, 56866, 86200, 41916, 94133, 34255, 20233, 79798, 54863, 1884, 85971, 73908, 62074, 9400, 61321, 79167, 546, 5206, 16758, 6047, 96169, 60475, 41905, 10006, 41812, 34379, 82546, 24525, 75745, 8874, 94645, 3955, 11491, 67223, 96378, 96917, 6759, 29104, 1381, 25473, 17227, 72214, 31343, 26706, 87691, 24556, 58705, 38400, 104991, 44800, 23035, 1419, 92486, 75763, 22074, 99065, 22524, 97232, 22525, 28508, 66430, 98637, 22226, 64250, 85319, 41834, 39079, 97503, 58757, 70909, 29507, 104447, 22151, 67695, 40479, 50517, 30374, 58785, 59718, 107465, 45250, 7711, 71564, 91365, 84589, 2634, 96346, 80538, 4631, 89207, 99358, 97595, 11422, 104877, 28913, 46545, 96491, 14761, 56420, 104752, 60402, 36288, 7512, 11035, 6866, 33311, 89158, 3293, 88146, 1252, 85234, 93076, 93394, 63216, 101341, 28634, 105187, 35880, 97857, 35792, 61872};
        path = find_path_to_point_of_interest(81143, "Subway", 15.00000000000000000);
        path_cost = compute_path_travel_time(path, 15.00000000000000000);
        CHECK(path_is_legal(81143, valid_end_intersections, path));
        CHECK(path_cost <= 63.47291184868886660);
        
        valid_end_intersections = {3128, 1127, 87701, 1130, 9511, 7276, 7245, 8227, 91425, 16773, 16695, 20024, 20228, 1750, 16570, 71078, 8374, 88345, 1118, 97185, 103427, 18672, 59292, 99983, 24740, 18487, 98904, 41909, 9163, 28915, 57095, 80708, 88674, 107653, 61242, 61372, 3305, 105178, 105429, 77832, 43589, 36000, 36298, 54326, 61296, 97143, 104617, 16757, 2600, 44754, 28907, 19182, 5510, 1671, 60902, 70584, 26925, 68574, 4308, 82642, 29789, 44161, 7854, 91776, 86833, 61226, 71731, 54006, 90874, 18970, 1078, 23758, 108379, 3679, 64275, 4461, 88344, 70573, 1744, 20826, 14901, 12572, 14675, 333, 5702, 44066, 71262, 98900, 4318, 24452, 99720, 71027, 57995, 5812, 13135, 749, 61308, 71869, 54499, 70708, 2246, 1862, 6753, 2294, 30964, 44035, 52383, 69685, 9799, 8172, 98946, 56866, 86200, 41916, 94133, 34255, 20233, 79798, 54863, 1884, 85971, 73908, 62074, 9400, 61321, 79167, 546, 5206, 16758, 6047, 96169, 60475, 41905, 10006, 41812, 34379, 82546, 24525, 75745, 8874, 94645, 3955, 11491, 67223, 96378, 96917, 6759, 29104, 1381, 25473, 17227, 72214, 31343, 26706, 87691, 24556, 58705, 38400, 104991, 44800, 23035, 1419, 92486, 75763, 22074, 99065, 22524, 97232, 22525, 28508, 66430, 98637, 22226, 64250, 85319, 41834, 39079, 97503, 58757, 70909, 29507, 104447, 22151, 67695, 40479, 50517, 30374, 58785, 59718, 107465, 45250, 7711, 71564, 91365, 84589, 2634, 96346, 80538, 4631, 89207, 99358, 97595, 11422, 104877, 28913, 46545, 96491, 14761, 56420, 104752, 60402, 36288, 7512, 11035, 6866, 33311, 89158, 3293, 88146, 1252, 85234, 93076, 93394, 63216, 101341, 28634, 105187, 35880, 97857, 35792, 61872};
        path = find_path_to_point_of_interest(27641, "Subway", 15.00000000000000000);
        path_cost = compute_path_travel_time(path, 15.00000000000000000);
        CHECK(path_is_legal(27641, valid_end_intersections, path));
        CHECK(path_cost <= 194.96582156498877225);
        
        valid_end_intersections = {23134, 26982, 62041, 25756, 16758, 1744, 5176};
        path = find_path_to_point_of_interest(82779, "Fit for Life", 15.00000000000000000);
        path_cost = compute_path_travel_time(path, 15.00000000000000000);
        CHECK(path_is_legal(82779, valid_end_intersections, path));
        CHECK(path_cost <= 431.75022359200073652);
        
        valid_end_intersections = {33234, 1868, 41447, 4013, 12572, 6865, 63067, 4182, 20024, 16674, 16695, 4303, 59289, 10082, 41904, 59277, 16982, 33019, 35886, 18798, 59406, 103957, 33014, 56839, 21364, 12581, 57046, 60374, 16767, 83299, 78935, 29524, 59016, 38971, 103250, 11750, 61608, 10068, 3678, 1896, 1792, 6860, 50908, 2061, 3305, 98160, 72374, 19132, 2248, 41886, 78403, 52556, 88597, 18680, 47984, 107997, 28501, 30394, 749, 62041, 16759, 6305, 71708, 67794, 16945, 70813, 88331, 94638, 87075, 12577, 1450, 26560, 50517, 3433, 92819, 5933, 1272, 94185, 24810, 884, 49119, 35236, 33477, 30558, 53003, 91411, 88593, 89809, 100644, 74058, 34613, 49443, 33048, 56243, 1862, 28335, 90858, 28635, 36679, 78526, 63217};
        path = find_path_to_point_of_interest(86691, "CIBC", 15.00000000000000000);
        path_cost = compute_path_travel_time(path, 15.00000000000000000);
        CHECK(path_is_legal(86691, valid_end_intersections, path));
        CHECK(path_cost <= 307.57890310666567757);
        
        valid_end_intersections = {6779, 5614, 3128, 80947, 28913, 56395, 26807, 93049, 71621, 98893, 94488, 104617, 95409, 62001, 11658, 3679, 70561, 104554, 70708, 78658, 76141, 79761, 105838, 94141, 69682, 11140, 92125, 70747, 79167, 56688, 27882, 16566, 30989, 36589, 4669, 84594, 104874, 55558, 95268, 75539, 58757, 97077, 32574, 97232, 1215, 71749, 1649, 96491, 50352};
        path = find_path_to_point_of_interest(49450, "Pizza Nova", 15.00000000000000000);
        path_cost = compute_path_travel_time(path, 15.00000000000000000);
        CHECK(path_is_legal(49450, valid_end_intersections, path));
        CHECK(path_cost <= 489.67647777318939006);
        
        valid_end_intersections = {16760};
        path = find_path_to_point_of_interest(88455, "Fresh Start Coffee Co.", 15.00000000000000000);
        path_cost = compute_path_travel_time(path, 15.00000000000000000);
        CHECK(path_is_legal(88455, valid_end_intersections, path));
        CHECK(path_cost <= 731.26504395304971240);
        
        valid_end_intersections = {12238, 6430, 7636, 61358, 104721, 5812, 100051, 47489, 15240, 44763, 18487, 29135, 71078, 28893, 19467, 88387, 60237, 26919, 61543, 32459, 30411, 83360, 8595, 28345, 56682, 59316, 58041, 89470, 57138, 7388, 26459, 29974, 86732, 44165, 17035, 35099, 71518, 78976, 14206, 54944, 78533, 62144, 61725, 74932, 43686, 58006, 57748, 14339, 60650, 37963, 52515, 70098, 97142, 89317, 105726, 8404, 77212, 66400, 59291, 30509, 82641, 33542, 81992, 82269, 20118, 50793, 89873, 100007, 23371, 28269, 107531, 98394, 54000, 54808, 55450, 51806, 1351, 55859, 90671, 92477, 1089, 55121, 95479, 69747, 53731, 87686, 54002, 56688, 76190, 70987, 55047, 77061, 63408, 59511, 60402, 64239, 75753, 77948, 77927, 92004, 60456, 87207, 24728, 61400, 322, 67438, 74302, 78617, 82751, 90682, 89317, 24864, 67871, 36512, 35246, 17338, 26347, 25516, 15594, 26609, 92879, 50506, 88706, 106895, 59185, 101114, 74779, 68628, 94397, 74578, 106127, 29414, 14484, 55712};
        path = find_path_to_point_of_interest(89779, "Petro-Canada", 15.00000000000000000);
        path_cost = compute_path_travel_time(path, 15.00000000000000000);
        CHECK(path_is_legal(89779, valid_end_intersections, path));
        CHECK(path_cost <= 244.75941552873084106);
        
        valid_end_intersections = {2352};
        path = find_path_to_point_of_interest(90256, "Butter Chicken Roti", 15.00000000000000000);
        path_cost = compute_path_travel_time(path, 15.00000000000000000);
        CHECK(path_is_legal(90256, valid_end_intersections, path));
        CHECK(path_cost <= 1872.84482282523686081);
        
        valid_end_intersections = {96941};
        path = find_path_to_point_of_interest(96760, "Convention Centre", 15.00000000000000000);
        path_cost = compute_path_travel_time(path, 15.00000000000000000);
        CHECK(path_is_legal(96760, valid_end_intersections, path));
        CHECK(path_cost <= 2040.51271795214825033);
        
        valid_end_intersections = {82830};
        path = find_path_to_point_of_interest(100931, "Morningstar Christian Fellowship", 15.00000000000000000);
        path_cost = compute_path_travel_time(path, 15.00000000000000000);
        CHECK(path_is_legal(100931, valid_end_intersections, path));
        CHECK(path_cost <= 2626.32452048137383827);
        
        valid_end_intersections = {1272, 99997, 11625, 22185, 88, 4820};
        path = find_path_to_point_of_interest(102771, "BMO Financial Group", 15.00000000000000000);
        path_cost = compute_path_travel_time(path, 15.00000000000000000);
        CHECK(path_is_legal(102771, valid_end_intersections, path));
        CHECK(path_cost <= 1555.49792905943445476);
        
        valid_end_intersections = {4111, 4015, 1401, 6304, 9511, 6718, 4137, 512, 2355, 70790, 2290, 15786, 1855, 759, 12544, 7765, 2543, 1014, 1868, 3468, 89255, 28892, 88345, 1118, 1656, 28591, 23132, 23220, 3701, 11016, 16749, 5176, 4183, 19143, 68577, 74994, 107653, 20244, 71793, 107821, 35792, 87129, 105429, 14691, 82479, 45670, 28896, 87695, 94740, 89171, 1896, 1744, 53720, 11454, 101715, 80730, 101976, 100230, 105568, 16941, 62042, 97497, 1078, 78903, 3172, 551, 70063, 70573, 70701, 70707, 89487, 11649, 8876, 98946, 4669, 69736, 16222, 28929, 52382, 9799, 28633, 2318, 20676, 2324, 4218, 91116, 2264, 86069, 45659, 80971, 30545, 101900, 1418, 15585, 94150, 79798, 5205, 89310, 12747, 85963, 98897, 41885, 71708, 34358, 87031, 2634, 20185, 12410, 100647, 79525, 14608, 1043, 99707, 33347, 1423, 96496, 33061, 71967, 38660, 100551, 94697, 95439, 79402, 75710, 61521, 34494, 34781, 99049, 64251, 85319, 21227, 11645, 85051, 106042, 19856, 2260, 3411, 15792, 34613, 19322, 44112, 3705, 86275, 71979, 99120, 6856, 20193, 30407, 2270, 2294, 5833, 73484, 104560, 20083, 18532, 369, 5852, 32615, 100120, 60407, 30537, 63236, 45673, 55062, 4321, 18888, 107893, 9313, 80267};
        path = find_path_to_point_of_interest(106261, "Starbucks Coffee", 15.00000000000000000);
        path_cost = compute_path_travel_time(path, 15.00000000000000000);
        CHECK(path_is_legal(106261, valid_end_intersections, path));
        CHECK(path_cost <= 295.73413310886576255);
        
        //Generate random inputs
        std::vector<unsigned> intersection_ids;
        std::vector<std::string> poi_names;
        std::vector<double> turn_penalties;
        for(size_t i = 0; i < 15; i++) {
            intersection_ids.push_back(rand_intersection(rng));
            poi_names.push_back(getPointOfInterestName(rand_poi(rng)));
            turn_penalties.push_back(rand_turn_penalty(rng));
        }
        {
            //Timed Test
            ECE297_TIME_CONSTRAINT(71870);
            std::vector<unsigned> result;
            for(size_t i = 0; i < 15; i++) {
                result = find_path_to_point_of_interest(intersection_ids[i], poi_names[i], turn_penalties[i]);
            }
        }
    } //find_path_to_point_of_interest_perf_basic

} //inter_inter_path_perf_basic_public

