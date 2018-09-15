#ifndef WAVESTABLES_H_INCLUDED
#define WAVESTABLES_H_INCLUDED

#include <vector>

namespace Wavetables
{

	extern const std::vector<double> sinTable;
	extern const std::vector<double> sinTableInterp0;
	extern const std::vector<double> sinTableInterp1;
	extern const std::vector<double> sinTableInterp2;
	extern const std::vector<double> sinTableInterp3;

    extern const std::vector<double> sinTableBipolar;
    extern const std::vector<double> sinTableInterp0Bipolar;
    extern const std::vector<double> sinTableInterp1Bipolar;
    extern const std::vector<double> sinTableInterp2Bipolar;
    extern const std::vector<double> sinTableInterp3Bipolar;

	extern const std::vector<double> triangleTable;
	extern const std::vector<double> triangleTableInterp0;
	extern const std::vector<double> triangleTableInterp1;
	extern const std::vector<double> triangleTableInterp2;
	extern const std::vector<double> triangleTableInterp3;

    extern const std::vector<double> triangleTableBipolar;
    extern const std::vector<double> triangleTableInterp0Bipolar;
    extern const std::vector<double> triangleTableInterp1Bipolar;
    extern const std::vector<double> triangleTableInterp2Bipolar;
    extern const std::vector<double> triangleTableInterp3Bipolar;

	extern const std::vector<double> sawtoothTable;
	extern const std::vector<double> sawtoothTableInterp0;
	extern const std::vector<double> sawtoothTableInterp1;
	extern const std::vector<double> sawtoothTableInterp2;
	extern const std::vector<double> sawtoothTableInterp3;

    extern const std::vector<double> sawtoothTableBipolar;
    extern const std::vector<double> sawtoothTableInterp0Bipolar;
    extern const std::vector<double> sawtoothTableInterp1Bipolar;
    extern const std::vector<double> sawtoothTableInterp2Bipolar;
    extern const std::vector<double> sawtoothTableInterp3Bipolar;

	extern const std::vector<double> reverseSawtoothTable;
	extern const std::vector<double> reverseSawtoothTableInterp0;
	extern const std::vector<double> reverseSawtoothTableInterp1;
	extern const std::vector<double> reverseSawtoothTableInterp2;
	extern const std::vector<double> reverseSawtoothTableInterp3;

    extern const std::vector<double> reverseSawtoothTableBipolar;
    extern const std::vector<double> reverseSawtoothTableInterp0Bipolar;
    extern const std::vector<double> reverseSawtoothTableInterp1Bipolar;
    extern const std::vector<double> reverseSawtoothTableInterp2Bipolar;
    extern const std::vector<double> reverseSawtoothTableInterp3Bipolar;

	extern const std::vector<double> squareTable;
	extern const std::vector<double> squareTableInterp0;
	extern const std::vector<double> squareTableInterp1;
	extern const std::vector<double> squareTableInterp2;
	extern const std::vector<double> squareTableInterp3;

    extern const std::vector<double> squareTableBipolar;
    extern const std::vector<double> squareTableInterp0Bipolar;
    extern const std::vector<double> squareTableInterp1Bipolar;
    extern const std::vector<double> squareTableInterp2Bipolar;
    extern const std::vector<double> squareTableInterp3Bipolar;
    /*
	extern const std::vector<double> exponentialTable;
	extern const std::vector<double> exponentialTableInterp0;
	extern const std::vector<double> exponentialTableInterp1;
	extern const std::vector<double> exponentialTableInterp2;
	extern const std::vector<double> exponentialTableInterp3;


	extern const std::vector<double> logarithmTable;
	extern const std::vector<double> logarithmTableInterp0;
	extern const std::vector<double> logarithmTableInterp1;
	extern const std::vector<double> logarithmTableInterp2;
	extern const std::vector<double> logarithmTableInterp3;


	extern const std::vector<double> rcChargeDischargeTable;
	extern const std::vector<double> rcChargeDischargeTableInterp0;
	extern const std::vector<double> rcChargeDischargeTableInterp1;
	extern const std::vector<double> rcChargeDischargeTableInterp2;
	extern const std::vector<double> rcChargeDischargeTableInterp3;


	extern const std::vector<double> tangentTable;
	extern const std::vector<double> tangentTableInterp0;
	extern const std::vector<double> tangentTableInterp1;
	extern const std::vector<double> tangentTableInterp2;
	extern const std::vector<double> tangentTableInterp3;


	extern const std::vector<double> halfRectifiedSinTable;
	extern const std::vector<double> halfRectifiedSinTableInterp0;
	extern const std::vector<double> halfRectifiedSinTableInterp1;
	extern const std::vector<double> halfRectifiedSinTableInterp2;
	extern const std::vector<double> halfRectifiedSinTableInterp3;


	extern const std::vector<double> fullRectifiedSinTable;
	extern const std::vector<double> fullRectifiedSinTableInterp0;
	extern const std::vector<double> fullRectifiedSinTableInterp1;
	extern const std::vector<double> fullRectifiedSinTableInterp2;
	extern const std::vector<double> fullRectifiedSinTableInterp3;


	extern const std::vector<double> squareRootTable;
	extern const std::vector<double> squareRootTableInterp0;
	extern const std::vector<double> squareRootTableInterp1;
	extern const std::vector<double> squareRootTableInterp2;
	extern const std::vector<double> squareRootTableInterp3;


	extern const std::vector<double> powerTwoTable;
	extern const std::vector<double> powerTwoTableInterp0;
	extern const std::vector<double> powerTwoTableInterp1;
	extern const std::vector<double> powerTwoTableInterp2;
	extern const std::vector<double> powerTwoTableInterp3;


	extern const std::vector<double> triangleIntegralTable;
	extern const std::vector<double> triangleIntegralTableInterp0;
	extern const std::vector<double> triangleIntegralTableInterp1;
	extern const std::vector<double> triangleIntegralTableInterp2;
	extern const std::vector<double> triangleIntegralTableInterp3;


	extern const std::vector<double> sincTable;
	extern const std::vector<double> sincTableInterp0;
	extern const std::vector<double> sincTableInterp1;
	extern const std::vector<double> sincTableInterp2;
	extern const std::vector<double> sincTableInterp3;


	extern const std::vector<double> staircaseTable;
	extern const std::vector<double> staircaseTableInterp0;
	extern const std::vector<double> staircaseTableInterp1;
	extern const std::vector<double> staircaseTableInterp2;
	extern const std::vector<double> staircaseTableInterp3;


	extern const std::vector<double> sinStaircaseTable;
	extern const std::vector<double>sinStaircaseTableInterp0;
	extern const std::vector<double> sinStaircaseTableInterp1;
	extern const std::vector<double> sinStaircaseTableInterp2;
	extern const std::vector<double> sinStaircaseTableInterp3;


	extern const std::vector<double> hannTable;
	extern const std::vector<double> hannTableInterp0;
	extern const std::vector<double> hannTableInterp1;
	extern const std::vector<double> hannTableInterp2;
	extern const std::vector<double> hannTableInterp3;


	extern const std::vector<double> gaussTable;
	extern const std::vector<double> gaussTableInterp0;
	extern const std::vector<double> gaussTableInterp1;
	extern const std::vector<double> gaussTableInterp2;
	extern const std::vector<double> gaussTableInterp3;


	extern const std::vector<double> bandlimitedSignalTable;
	extern const std::vector<double> bandlimitedSignalTableInterp0;
	extern const std::vector<double> bandlimitedSignalTableInterp1;
	extern const std::vector<double> bandlimitedSignalTableInterp2;
	extern const std::vector<double> bandlimitedSignalTableInterp3;


	extern const std::vector<double> bandlimitedImpulseTable;
	extern const std::vector<double> bandlimitedImpulseTableInterp0;
	extern const std::vector<double> bandlimitedImpulseTableInterp1;
	extern const std::vector<double> bandlimitedImpulseTableInterp2;
	extern const std::vector<double> bandlimitedImpulseTableInterp3;


	extern const std::vector<double> bandlimitedTriangleTable;
	extern const std::vector<double> bandlimitedTriangleTableInterp0;
	extern const std::vector<double> bandlimitedTriangleTableInterp1;
	extern const std::vector<double> bandlimitedTriangleTableInterp2;
	extern const std::vector<double> bandlimitedTriangleTableInterp3;


	extern const std::vector<double> bandlimitedSawtoothTable;
	extern const std::vector<double> bandlimitedSawtoothTableInterp0;
	extern const std::vector<double> bandlimitedSawtoothTableInterp1;
	extern const std::vector<double> bandlimitedSawtoothTableInterp2;
	extern const std::vector<double> bandlimitedSawtoothTableInterp3;


	extern const std::vector<double> bandlimitedReverseSawtoothTable;
	extern const std::vector<double> bandlimitedReverseSawtoothTableInterp0;
	extern const std::vector<double> bandlimitedReverseSawtoothTableInterp1;
	extern const std::vector<double> bandlimitedReverseSawtoothTableInterp2;
	extern const std::vector<double> bandlimitedReverseSawtoothTableInterp3;


	extern const std::vector<double> bandlimitedSquareTable;
	extern const std::vector<double> bandlimitedSquareTableInterp0;
	extern const std::vector<double> bandlimitedSquareTableInterp1;
	extern const std::vector<double> bandlimitedSquareTableInterp2;
	extern const std::vector<double> bandlimitedSquareTableInterp3;
    */
     
}


#endif 
