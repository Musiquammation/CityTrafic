#pragma once

enum class JobOfferType {
    OIL_RAFFINER,
    AGRICULTOR,
    CASHIER,
    CONSTRUCTION
};

struct JobOffer {
    JobOfferType type;
    int salaryEstimation;
};
