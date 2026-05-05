#pragma once

enum class JobOfferType {
    OIL_RAFFINER,
    AGRICULTOR,
    CASHIER
};

struct JobOffer {
    JobOfferType type;
    int salaryEstimation;
};
