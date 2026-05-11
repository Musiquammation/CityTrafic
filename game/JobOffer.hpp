#pragma once

enum class JobOfferType {
    OIL_RAFFINER,
    AGRICULTOR,
    CASHIER,
    CONSTRUCTION,
    TRUCK
};

struct JobOffer {
    JobOfferType type;
    int salaryEstimation;
};
