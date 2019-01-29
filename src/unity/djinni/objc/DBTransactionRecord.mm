// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from libunity.djinni

#import "DBTransactionRecord.h"


@implementation DBTransactionRecord

- (nonnull instancetype)initWithTxHash:(nonnull NSString *)txHash
                             timestamp:(int64_t)timestamp
                                amount:(int64_t)amount
                                   fee:(int64_t)fee
                                status:(DBTransactionStatus)status
                                height:(int32_t)height
                                 depth:(int32_t)depth
                       receivedOutputs:(nonnull NSArray<DBOutputRecord *> *)receivedOutputs
                           sentOutputs:(nonnull NSArray<DBOutputRecord *> *)sentOutputs
{
    if (self = [super init]) {
        _txHash = [txHash copy];
        _timestamp = timestamp;
        _amount = amount;
        _fee = fee;
        _status = status;
        _height = height;
        _depth = depth;
        _receivedOutputs = [receivedOutputs copy];
        _sentOutputs = [sentOutputs copy];
    }
    return self;
}

+ (nonnull instancetype)transactionRecordWithTxHash:(nonnull NSString *)txHash
                                          timestamp:(int64_t)timestamp
                                             amount:(int64_t)amount
                                                fee:(int64_t)fee
                                             status:(DBTransactionStatus)status
                                             height:(int32_t)height
                                              depth:(int32_t)depth
                                    receivedOutputs:(nonnull NSArray<DBOutputRecord *> *)receivedOutputs
                                        sentOutputs:(nonnull NSArray<DBOutputRecord *> *)sentOutputs
{
    return [[self alloc] initWithTxHash:txHash
                              timestamp:timestamp
                                 amount:amount
                                    fee:fee
                                 status:status
                                 height:height
                                  depth:depth
                        receivedOutputs:receivedOutputs
                            sentOutputs:sentOutputs];
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"<%@ %p txHash:%@ timestamp:%@ amount:%@ fee:%@ status:%@ height:%@ depth:%@ receivedOutputs:%@ sentOutputs:%@>", self.class, (void *)self, self.txHash, @(self.timestamp), @(self.amount), @(self.fee), @(self.status), @(self.height), @(self.depth), self.receivedOutputs, self.sentOutputs];
}

@end